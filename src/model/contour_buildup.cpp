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
			BaseModel3D( VTX::ID::Model::MODEL_CONTOUR_BUILDUP ), _molecule( p_molecule )
		{
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

		void ContourBuildup::_init()
		{ // _computeGPU();
		}

		void ContourBuildup::_fillBuffer() {}

		void ContourBuildup::_computeAABB() const {}

		void ContourBuildup::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Torus>( this, VTX::ID::View::D3_TORUS_PATCH ) );
			_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::SphericalTriangle>(
				this, VTX::ID::View::D3_SPHERICAL_TRIANGLE_PATCH ) );
		}

		void ContourBuildup::_computeGPU()
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating contour buildup structure..." );

			{
				// Setup acceleration grid.
				Math::Grid		   accelerationGrid = Math::Grid();
				const Math::AABB & molAABB			= _molecule->getAABB();
				const float		   maxVdwRadius		= *std::max_element(
					   Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );

				accelerationGrid.worldOrigin = molAABB.getMin() - maxVdwRadius - ProbeRadius;
				const Vec3f worldSize
					= glm::abs( molAABB.getMax() + maxVdwRadius + ProbeRadius - accelerationGrid.worldOrigin );

				const std::size_t gridSize = Util::Math::nextPowerOfTwoValue(
					Util::Math::nextPowerOfTwoExponent( _molecule->getAtomCount() ) );
				accelerationGrid.size	  = Vec3i( int( gridSize ) );
				accelerationGrid.cellSize = Vec3f( worldSize / Vec3f( accelerationGrid.size ) );

				// The following code is based on the Megamol implementation
				// https://github.com/UniStuttgart-VISUS/megamol/blob/master/plugins/protein_cuda/src/MoleculeCBCudaRenderer.cpp#L901
				const std::vector<Model::Atom *> & atoms = _molecule->getAtoms();
				const std::vector<Vec3f> atomsPositions	 = _molecule->getAtomPositionFrame( _molecule->getFrame() );
				const uint				 paddedSize		 = Util::Math::nextPowerOfTwoValue( uint( atoms.size() ) );

				// Bitonic sorter lambda.
				const auto bitonicSort = []( const IO::FilePath & p_sortingProgramPath,
											 const uint			  p_arraySize,
											 const uint			  p_maxWorkGroupSize = 512 )
				{
					assert( p_arraySize % 2 == 0 );
					if ( p_arraySize < 2 )
						return;

					uint workGroupWidth = 1;
					if ( p_arraySize < p_maxWorkGroupSize * 2 )
					{
						workGroupWidth = p_arraySize / 2;
					}
					else
					{
						workGroupWidth = p_maxWorkGroupSize;
					}

					VTX::Worker::GpuComputer bitonicSort = VTX::Worker::GpuComputer(
						p_sortingProgramPath, Vec3i( workGroupWidth, 1, 1 ), GL_SHADER_STORAGE_BARRIER_BIT, false );

					const uint32_t workgroup_count = uint32_t( p_arraySize / ( workGroupWidth * 2 ) );
					uint32_t	   h			   = uint32_t( workGroupWidth ) * 2;

					assert( h <= p_arraySize );
					assert( h % 2 == 0 );

					// Local Bms
					bitonicSort.getProgram().use();
					bitonicSort.getProgram().setUInt( "n", p_arraySize );
					bitonicSort.getProgram().setUInt( "h", h );
					bitonicSort.getProgram().setUInt( "algorithm", 0u );
					bitonicSort.start();

					// we must now double h, as this happens before every flip
					h *= 2;

					for ( ; h <= p_arraySize; h *= 2 )
					{
						// big_flip
						bitonicSort.getProgram().use();
						bitonicSort.getProgram().setUInt( "h", h );
						bitonicSort.getProgram().setUInt( "algorithm", 2u );
						bitonicSort.start();

						for ( uint32_t hh = h / 2; hh > 1; hh /= 2 )
						{
							if ( hh <= workGroupWidth * 2 )
							{
								// local_disperse
								// We can fit all elements for a disperse operation into continuous shader
								// workgroup local memory, which means we can complete the rest of the
								// cascade using a single shader invocation.
								bitonicSort.getProgram().use();
								bitonicSort.getProgram().setUInt( "h", hh );
								bitonicSort.getProgram().setUInt( "algorithm", 1u );
								bitonicSort.start();

								break;
							}
							else
							{
								// big_disperse
								bitonicSort.getProgram().use();
								bitonicSort.getProgram().setUInt( "h", hh );
								bitonicSort.getProgram().setUInt( "algorithm", 3u );
								bitonicSort.start();
							}
						}
					}
				};

				VTX::Renderer::GL::BufferStorage ssboAtomPosition = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboAtomPosition.create();

				std::vector<AtomInformation> atomsInformations = std::vector<AtomInformation>();
				atomsInformations.reserve( _molecule->getAtomCount() );
				for ( uint i = 0; i < atoms.size(); i++ )
				{
					const Vec3f & currentPosition = atomsPositions[ i ];
					const Vec3i	  gridPos		  = accelerationGrid.gridPosition( currentPosition );
					const uint	  hash			  = accelerationGrid.gridHash( gridPos );

					atomsInformations.emplace_back(
						AtomInformation { currentPosition, atoms[ i ]->getVdwRadius(), 0, 0, hash } );
				}

				for ( uint i = uint( atoms.size() ); i < paddedSize; i++ )
				{
					atomsInformations.emplace_back(
						AtomInformation { Vec3f( 0.f ), 0, 0, 0, std::numeric_limits<VTX::uint>::max() } );
				}

				std::vector<uint8_t> atomPositionData = std::vector<uint8_t>(
					sizeof( uint ) + /* padding */ 12 + atomsInformations.size() * sizeof( AtomInformation ) );
				const uint atomNb = uint( atoms.size() );
				std::memcpy( atomPositionData.data(), &atomNb, sizeof( uint ) );
				std::memcpy( atomPositionData.data() + sizeof( uint ) + 12,
							 atomsInformations.data(),
							 atomsInformations.size() * sizeof( AtomInformation ) );
				ssboAtomPosition.set( atomPositionData );

				ssboAtomPosition.bind( 0 );
				bitonicSort( IO::FilePath( "sort_atoms.comp" ), paddedSize );
				ssboAtomPosition.unbind();

				// workerBuildGrid.
				VTX::Worker::GpuComputer workerBuildGrid
					= VTX::Worker::GpuComputer( IO::FilePath( "build_atoms_grid.comp" ),
												uint( atomsInformations.size() ),
												GL_SHADER_STORAGE_BARRIER_BIT );

				std::vector<GridCellInfo> atomGridIndices = std::vector<GridCellInfo>(
					accelerationGrid.size.x * accelerationGrid.size.y * accelerationGrid.size.z,
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
				ssboCircles.set( std::vector<uint8_t>( circlePreAllocationSize, 0u ),
								 VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );

				workerNeighborsCount.getProgram().use();
				workerNeighborsCount.getProgram().setInt( "uMaxNeighborsNb", MaxAtomNeighborNb );
				workerNeighborsCount.getProgram().setFloat( "uProbeRadius", ProbeRadius );
				workerNeighborsCount.getProgram().setVec3i( "uGridSize", accelerationGrid.size );
				workerNeighborsCount.getProgram().setVec3f( "uCellSize", accelerationGrid.cellSize );
				workerNeighborsCount.getProgram().setVec3f( "uWorldOrigin", accelerationGrid.worldOrigin );
				workerNeighborsCount.getProgram().setFloat( "uMaxVdwRadius", maxVdwRadius );

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
				assert( realCircleNb != nullptr );
				const uint torusPatchNb = uint( *realCircleNb );
				assert( torusPatchNb > 0 );
				ssboCircles.unmap();
				ssboCircles.unbind();

				const uint circleProbePreAllocationSize
					= sizeof( uint ) + 12 + sizeof( ProbePosition ) * torusPatchNb * MaxProbePerCircle;

				VTX::Renderer::GL::BufferStorage ssboCirclesProbes = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboCirclesProbes.create();
				ssboCirclesProbes.set( std::vector<uint8_t>( circlePreAllocationSize, 0u ),
									   VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );

				// workerContourBuildup.
				VTX::Worker::GpuComputer workerContourBuildup = VTX::Worker::GpuComputer(
					IO::FilePath( "contour_buildup.comp" ), torusPatchNb, GL_SHADER_STORAGE_BARRIER_BIT );
				workerContourBuildup.getProgram().use();
				workerContourBuildup.getProgram().setInt( "uMaxNeighborsNb", MaxAtomNeighborNb );
				workerContourBuildup.getProgram().setFloat( "uProbeRadius", ProbeRadius );
				workerContourBuildup.getProgram().setInt( "uMaxProbePerCircle", MaxProbePerCircle );

				ssboAtomPosition.bind( 0 );
				ssboCircles.bind( 1 );
				ssboCirclesProbes.bind( 2 );

				// Start a thread per circle
				workerContourBuildup.start();

				ssboAtomPosition.unbind();
				ssboCircles.unbind();
				ssboCirclesProbes.unbind();

				ssboCirclesProbes.bind();
				const uint * const probeNb = ssboCirclesProbes.map<uint>(
					0, sizeof( uint ), VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );
				assert( probeNb != nullptr );
				const uint trianglePatchNb = uint( *probeNb );
				assert( trianglePatchNb > 0 );
				ssboCirclesProbes.unmap();
				ssboCirclesProbes.unbind();

				const VTX::Renderer::GL::BufferStorage & ssboTorusPatches = _buffer->getSsboTorusPatches();
				ssboTorusPatches.set(
					std::vector<uint8_t>( sizeof( Buffer::ContourBuildup::TorusPatch ) * torusPatchNb, 0u ) );
				uint paddedNbOfSpTriangle = Util::Math::nextPowerOfTwoValue( trianglePatchNb );

				std::vector<uint8_t> trianglePatchesInit = std::vector<uint8_t>(
					sizeof( uint ) + 12
					+ sizeof( Buffer::ContourBuildup::SphericalTrianglePatch ) * paddedNbOfSpTriangle );

				const std::vector<Buffer::ContourBuildup::SphericalTrianglePatch> trianglesPatchesDefault
					= std::vector<Buffer::ContourBuildup::SphericalTrianglePatch>(
						paddedNbOfSpTriangle,
						Buffer::ContourBuildup::SphericalTrianglePatch {
							Vec4f( 0.f ),
							Vec4f( 0.f ),
							Vec4f( 0.f ),
							Vec4f( 0.f, 0.f, 0.f, static_cast<float>( std::numeric_limits<uint>::max() ) ) } );
				std::memcpy( trianglePatchesInit.data() + sizeof( uint ) + 12,
							 trianglesPatchesDefault.data(),
							 sizeof( Buffer::ContourBuildup::SphericalTrianglePatch ) * paddedNbOfSpTriangle );

				const VTX::Renderer::GL::BufferStorage & ssboSphericalTrianglePatches
					= _buffer->getSsboSphericalTrianglePatches();
				ssboSphericalTrianglePatches.set( trianglePatchesInit );

				// workerBuildPatch.
				VTX::Worker::GpuComputer workerBuildPatch = VTX::Worker::GpuComputer(
					IO::FilePath( "build_patch.comp" ), torusPatchNb, GL_SHADER_STORAGE_BARRIER_BIT );
				workerBuildPatch.getProgram().use();
				workerBuildPatch.getProgram().setFloat( "uProbeRadius", ProbeRadius );
				workerBuildPatch.getProgram().setVec3i( "uGridSize", accelerationGrid.size );
				workerBuildPatch.getProgram().setVec3f( "uCellSize", accelerationGrid.cellSize );
				workerBuildPatch.getProgram().setVec3f( "uWorldOrigin", accelerationGrid.worldOrigin );

				ssboAtomPosition.bind( 0 );
				ssboCircles.bind( 1 );
				ssboCirclesProbes.bind( 2 );
				ssboTorusPatches.bind( 3 );
				ssboSphericalTrianglePatches.bind( 4 );

				// Start a thread per circle
				workerBuildPatch.start();

				ssboAtomPosition.unbind();
				ssboCircles.unbind();
				ssboCirclesProbes.unbind();
				ssboTorusPatches.unbind();
				ssboSphericalTrianglePatches.unbind();

				ssboSphericalTrianglePatches.bind( 0 );
				bitonicSort( IO::FilePath( "sort_probes.comp" ), paddedNbOfSpTriangle, 256 );
				ssboSphericalTrianglePatches.unbind();

				std::vector<GridCellInfo> probesGridIndices = std::vector<GridCellInfo>(
					uint( accelerationGrid.size.x ) * accelerationGrid.size.y * accelerationGrid.size.z,
					GridCellInfo { -1, -1 } );

				VTX::Renderer::GL::BufferStorage ssboProbesCellInfo = VTX::Renderer::GL::BufferStorage(
					VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
				ssboProbesCellInfo.create();
				ssboProbesCellInfo.set( probesGridIndices );

				// workerBuildPatch.
				VTX::Worker::GpuComputer workerBuildProbesGrid = VTX::Worker::GpuComputer(
					IO::FilePath( "build_probes_grid.comp" ), trianglePatchNb, GL_SHADER_STORAGE_BARRIER_BIT );

				ssboSphericalTrianglePatches.bind( 0 );
				ssboProbesCellInfo.bind( 2 );
				workerBuildProbesGrid.start();
				ssboProbesCellInfo.unbind();
				ssboSphericalTrianglePatches.unbind();

				const VTX::Renderer::GL::BufferStorage & ssboProbeIntersection = _buffer->getSsboProbeIntersection();
				ssboProbeIntersection.set(
					std::vector<uint8_t>( sizeof( Vec4f ) * trianglePatchNb * MaxProbeNeighborNb, 0u ) );

				// workerBuildSingularityBuffer.
				VTX::Worker::GpuComputer workerBuildSingularityBuffer = VTX::Worker::GpuComputer(
					IO::FilePath( "build_singularity_buffer.comp" ), trianglePatchNb, GL_SHADER_STORAGE_BARRIER_BIT );

				// Fill intersection texture
				workerBuildSingularityBuffer.getProgram().use();
				workerBuildSingularityBuffer.getProgram().setFloat( "uProbeRadius", ProbeRadius );
				workerBuildSingularityBuffer.getProgram().setUInt( "uMaxProbeNeighborNb", MaxProbeNeighborNb );
				workerBuildSingularityBuffer.getProgram().setVec3i( "uGridSize", accelerationGrid.size );
				workerBuildSingularityBuffer.getProgram().setVec3f( "uCellSize", accelerationGrid.cellSize );
				workerBuildSingularityBuffer.getProgram().setVec3f( "uWorldOrigin", accelerationGrid.worldOrigin );

				ssboSphericalTrianglePatches.bind( 0 );
				ssboProbeIntersection.bind( 1 );
				ssboProbesCellInfo.bind( 2 );

				// Start a thread per circle
				workerBuildSingularityBuffer.start();

				ssboSphericalTrianglePatches.unbind();
				ssboProbeIntersection.unbind();
				ssboProbesCellInfo.unbind();
			}

			chrono.stop();
			VTX_INFO( "Contour buildup structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}
	} // namespace Model
} // namespace VTX
