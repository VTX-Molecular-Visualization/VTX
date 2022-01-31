#include "contour_buildup.hpp"
#include "atom.hpp"
#include "molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/buffer_storage.hpp"
#include "tool/chrono.hpp"
#include "view/d3/spherical_triangle.hpp"
#include "view/d3/torus.hpp"
#include "worker/gpu_computer.hpp"

namespace VTX
{
	namespace Model
	{
		ContourBuildup::ContourBuildup( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_CONTOUR_BUILDUP ), _molecule( p_molecule ),
			// ??
			_probeRadius( 0 )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating contour buildup structure..." );

			{
				// The following code is based on the Megamol implementation
				// https://github.com/UniStuttgart-VISUS/megamol/blob/master/plugins/protein_cuda/src/MoleculeCBCudaRenderer.cpp#L901
				const auto &			 atoms			= _molecule->getAtoms();
				const std::vector<Vec3f> atomsPositions = _molecule->getAtomPositionFrame( _molecule->getFrame() );
				const uint				 paddedSize		= Util::Math::nextPowerOfTwoValue( uint( atoms.size() ) );

				const auto bitonicSort = []( const IO::FilePath & sortingProgramPath,
											 const uint			  arraySize,
											 const uint			  MaxWorkGroupSize = 512 )
				{
					assert( arraySize % 2 == 0 );
					if ( arraySize < 2 )
						return;

					uint workGroupWidth = 1;
					if ( arraySize < MaxWorkGroupSize * 2 )
					{
						workGroupWidth = arraySize / 2;
					}
					else
					{
						workGroupWidth = MaxWorkGroupSize;
					}

					VTX::Worker::GpuComputer bitonicSort = VTX::Worker::GpuComputer(
						sortingProgramPath, Vec3i( workGroupWidth, 1, 1 ), GL_SHADER_STORAGE_BARRIER_BIT, false );

					const uint32_t workgroup_count = uint32_t( arraySize / ( workGroupWidth * 2 ) );
					uint32_t	   h			   = uint32_t( workGroupWidth ) * 2;

					assert( h <= arraySize );
					assert( h % 2 == 0 );

					// Local Bms

					bitonicSort.getProgram().setInt( "n", uint( arraySize ) );
					bitonicSort.getProgram().setInt( "h", h );
					bitonicSort.getProgram().setInt( "algorithm", 0u );
					bitonicSort.start();

					// we must now double h, as this happens before every flip
					h *= 2;

					for ( ; h <= arraySize; h *= 2 )
					{
						// big_flip
						bitonicSort.getProgram().setInt( "h", h );
						bitonicSort.getProgram().setInt( "algorithm", 2u );
						bitonicSort.start();

						for ( uint32_t hh = h / 2; hh > 1; hh /= 2 )
						{
							if ( hh <= workGroupWidth * 2 )
							{
								// local_disperse
								// We can fit all elements for a disperse operation into continuous shader
								// workgroup local memory, which means we can complete the rest of the
								// cascade using a single shader invocation.
								bitonicSort.getProgram().setInt( "h", hh );
								bitonicSort.getProgram().setInt( "algorithm", 1u );
								bitonicSort.start();

								break;
							}
							else
							{
								// big_disperse
								bitonicSort.getProgram().setInt( "h", hh );
								bitonicSort.getProgram().setInt( "algorithm", 3u );
								bitonicSort.start();
							}
						}
					}
				};

				VTX::Renderer::GL::BufferStorage ssboAtomPosition = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboAtomPosition.create();
				_atomsInformations.reserve( _molecule->getAtomCount() );
				for ( uint i = 0; i < atoms.size(); i++ )
				{
					const auto & currentPosition = atomsPositions[ i ];
					const Vec3i	 gridPos		 = _accelerationGrid.gridPosition( currentPosition );
					const uint	 hash			 = _accelerationGrid.gridHash( gridPos );

					_atomsInformations.emplace_back(
						AtomInformation { currentPosition, atoms[ i ]->getVdwRadius(), 0, 0, hash } );
				}

				for ( uint i = uint( atoms.size() ); i < paddedSize; i++ )
				{
					_atomsInformations.emplace_back(
						AtomInformation { Vec3f( 0.f ), 0, 0, 0, std::numeric_limits<VTX::uint>::max() } );
				}

				std::vector<uint8_t> atomPositionData = std::vector<uint8_t>(
					sizeof( uint ) + /* padding */ 12 + _atomsInformations.size() * sizeof( AtomInformation ) );
				const uint atomNb = uint( atoms.size() );
				std::memcpy( atomPositionData.data(), &atomNb, sizeof( uint ) );
				std::memcpy( atomPositionData.data() + sizeof( uint ) + 12,
							 _atomsInformations.data(),
							 _atomsInformations.size() * sizeof( AtomInformation ) );
				ssboAtomPosition.set( atomPositionData );

				ssboAtomPosition.bind( 0 );
				bitonicSort( IO::FilePath( "sort_atoms.comp" ), paddedSize );
				ssboAtomPosition.unbind();

				// workerBuildGrid.
				VTX::Worker::GpuComputer workerBuildGrid
					= VTX::Worker::GpuComputer( IO::FilePath( "build_atoms_grid.comp" ),
												uint( _atomsInformations.size() ),
												GL_SHADER_STORAGE_BARRIER_BIT );

				std::vector<GridCellInfo> atomGridIndices = std::vector<GridCellInfo>(
					_accelerationGrid.size.x * _accelerationGrid.size.y * _accelerationGrid.size.z,
					GridCellInfo { -1, -1 } );
				VTX::Renderer::GL::BufferStorage ssboAtomCellInfo = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboAtomCellInfo.create();
				ssboAtomCellInfo.set( atomGridIndices );
				ssboAtomPosition.bind( 0 );
				ssboAtomCellInfo.bind( 2 );
				workerBuildGrid.start();
				ssboAtomCellInfo.unbind();
				ssboAtomPosition.unbind();

				// workerNeighborsCount.
				VTX::Worker::GpuComputer workerNeighborsCount = VTX::Worker::GpuComputer(
					IO::FilePath( "count_neighbors.comp" ), uint( atoms.size() ), GL_SHADER_STORAGE_BARRIER_BIT );

				const uint circleMaximumNb = uint( atoms.size() ) * MaxAtomNeighborNb;
				const uint circlePreAllocationSize
					= sizeof( uint ) + /* padding */ 12 + sizeof( CircleIntersection ) * circleMaximumNb;
				VTX::Renderer::GL::BufferStorage ssboCircles = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboCircles.create();
				ssboCircles.set( std::vector<uint8_t>( circlePreAllocationSize, 0u ) );

				workerNeighborsCount.getProgram().setInt( "uMaxNeighborsNb", MaxAtomNeighborNb );
				workerNeighborsCount.getProgram().setInt( "uProbeRadius", _probeRadius );
				workerNeighborsCount.getProgram().setVec3i( "uGridSize", _accelerationGrid.size );
				workerNeighborsCount.getProgram().setVec3f( "uCellSize", _accelerationGrid.cellSize );
				workerNeighborsCount.getProgram().setVec3f( "uWorldOrigin", _accelerationGrid.worldOrigin );
				const float maxVdwRadius = *std::max_element(
					Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
				workerNeighborsCount.getProgram().setInt( "uMaxVdwRadius", maxVdwRadius );

				ssboAtomPosition.bind( 0 );
				ssboCircles.bind( 1 );
				ssboAtomCellInfo.bind( 2 );
				workerNeighborsCount.start();
				ssboAtomPosition.unbind();
				ssboCircles.unbind();
				ssboAtomCellInfo.unbind();

				ssboCircles.bind();
				const uint * const realCircleNb
					= ssboCircles.map<uint>( 0, sizeof( uint ), VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );
				auto torusPatchNb = uint( *realCircleNb );
				ssboCircles.unmap();
				ssboCircles.unbind();

				/*
				const uint circleProbePreAllocationSize
					= sizeof( uint ) + 12 + sizeof( ProbePosition ) * _torusPatchNb * MaxProbePerCircle;

				std::unique_ptr<GpuBuffer> ssboCirclesProbes = std::make_unique<GpuBuffer>(
					circleProbePreAllocationSize, Renderer::BufferType::SSBO, Renderer::BufferAuthorization::READ );

				_contourBuildupWorker->getUniformManager().addValue( "uMaxNeighborsNb",
																	 static_cast<GLint>( MaxAtomNeighborNb ) );
				_contourBuildupWorker->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				_contourBuildupWorker->getUniformManager().addValue( "uMaxProbePerCircle",
																	 static_cast<GLint>( MaxProbePerCircle ) );

				ssboAtomPosition->bind( 0 );
				ssboCircles->bind( 1 );
				ssboCirclesProbes->bind( 2 );

				// Start a thread per circle
				_contourBuildupWorker->start( _torusPatchNb, { GL_SHADER_STORAGE_BARRIER_BIT } );

				ssboAtomPosition->unbind();
				ssboCircles->unbind();
				ssboCirclesProbes->unbind();

				const uint * const probeNb
					= ssboCirclesProbes->map<uint>( 0, sizeof( uint ), Renderer::BufferAuthorization::READ );
				_trianglePatchNb = uint( *probeNb );
				ssboCirclesProbes->unmap();

				_ssboTorusPatches		  = std::make_unique<GpuBuffer>( sizeof( TorusPatch ) * _torusPatchNb );
				uint paddedNbOfSpTriangle = Util::Math::nextPowerOfTwoValue( _trianglePatchNb );

				auto trianglePatchesInit = std::vector<uint8_t>(
					sizeof( uint ) +  12 + sizeof( SphericalTrianglePatch ) * paddedNbOfSpTriangle );
				{
					const auto trianglesPatchesDefault = std::vector<SphericalTrianglePatch>(
						paddedNbOfSpTriangle,
						SphericalTrianglePatch {
							Vec4f( 0.f ),
							Vec4f( 0.f ),
							Vec4f( 0.f ),
							Vec4f( 0.f, 0.f, 0.f, static_cast<float>( std::numeric_limits<uint>::max() ) ) } );
					std::memcpy( trianglePatchesInit.data() + sizeof( uint ) + 12,
								 trianglesPatchesDefault.data(),
								 sizeof( SphericalTrianglePatch ) * paddedNbOfSpTriangle );
				}
				_ssboSpTrianglePatches = std::make_unique<GpuBuffer>( trianglePatchesInit );

				_buildPatchWorker->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				_buildPatchWorker->getUniformManager().addValue( "uGridSize", _accelerationGrid.size );
				_buildPatchWorker->getUniformManager().addValue( "uCellSize", _accelerationGrid.cellSize );
				_buildPatchWorker->getUniformManager().addValue( "uWorldOrigin", _accelerationGrid.worldOrigin );

				ssboAtomPosition->bind( 0 );
				ssboCircles->bind( 1 );
				ssboCirclesProbes->bind( 2 );
				_ssboTorusPatches->bind( 3 );
				_ssboSpTrianglePatches->bind( 4 );

				// Start a thread per circle
				_buildPatchWorker->start( _torusPatchNb, { GL_SHADER_STORAGE_BARRIER_BIT } );

				ssboAtomPosition->unbind();
				ssboCircles->unbind();
				ssboCirclesProbes->unbind();
				_ssboTorusPatches->unbind();
				_ssboSpTrianglePatches->unbind();

				// We no longer needs those buffers
				ssboAtomPosition.reset();
				ssboCircles.reset();
				ssboCirclesProbes.reset();

				_ssboSpTrianglePatches->bind( 0 );
				bitonicSort( "contourbuildup/compute/sort_probes.comp", paddedNbOfSpTriangle, 256 );
				_ssboSpTrianglePatches->unbind();

				auto probesGridIndices = std::vector<GridCellInfo>(
					uint( _accelerationGrid.size.x ) * _accelerationGrid.size.y * _accelerationGrid.size.z,
					GridCellInfo { -1, -1 } );
				auto ssboProbesCellInfo = std::make_unique<GpuBuffer>( probesGridIndices );

				_ssboSpTrianglePatches->bind( 0 );
				ssboProbesCellInfo->bind( 2 );
				_buildProbesGridWorker->start( _trianglePatchNb, { GL_SHADER_STORAGE_BARRIER_BIT } );
				ssboProbesCellInfo->unbind();
				_ssboSpTrianglePatches->unbind();

				_ssboProbeIntersection
					= std::make_unique<GpuBuffer>( sizeof( Vec4f ) * _trianglePatchNb * MaxProbeNeighborNb );

				// Fill intersection texture
				_buildSingularityBufferWorker->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				_buildSingularityBufferWorker->getUniformManager().addValue( "uMaxProbeNeighborNb",
																			 static_cast<int>( MaxProbeNeighborNb ) );
				_buildSingularityBufferWorker->getUniformManager().addValue( "uGridSize", _accelerationGrid.size );
				_buildSingularityBufferWorker->getUniformManager().addValue( "uCellSize", _accelerationGrid.cellSize );
				_buildSingularityBufferWorker->getUniformManager().addValue( "uWorldOrigin",
																			 _accelerationGrid.worldOrigin );

				_ssboSpTrianglePatches->bind( 0 );
				_ssboProbeIntersection->bind( 1 );
				ssboProbesCellInfo->bind( 2 );

				// Start a thread per circle
				_buildSingularityBufferWorker->start( _trianglePatchNb, { GL_SHADER_STORAGE_BARRIER_BIT } );

				_ssboSpTrianglePatches->unbind();
				_ssboProbeIntersection->unbind();
				ssboProbesCellInfo->unbind();
				*/
			}

			chrono.stop();
			VTX_INFO( "Contour buildup structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void ContourBuildup::render( const Object3D::Camera & p_camera ) const
		{
			if ( _buffer == nullptr )
				return;

			_buffer->bind();
			_buffer->getSsboTorusPatches().bind( 0 );
			MVC::MvcManager::get().getView<View::D3::Torus>( this, VTX::ID::View::D3_TORUS_PATCH )->render( p_camera );
			_buffer->getSsboSphericalTrianglePatches().bind( 0 );
			_buffer->getSsboProbeIntersection().bind( 1 );
			MVC::MvcManager::get()
				.getView<View::D3::SphericalTriangle>( this, VTX::ID::View::D3_SPHERICAL_TRIANGLE_PATCH )
				->render( p_camera );
			_buffer->unbind();
		}

		void ContourBuildup::print() const {}

		void ContourBuildup::_init() {}

		void ContourBuildup::_fillBuffer() {}

		void ContourBuildup::_computeAABB() const {}

		void ContourBuildup::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Torus>( this, VTX::ID::View::D3_TORUS_PATCH ) );
			_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::SphericalTriangle>(
				this, VTX::ID::View::D3_SPHERICAL_TRIANGLE_PATCH ) );
		}
	} // namespace Model
} // namespace VTX
