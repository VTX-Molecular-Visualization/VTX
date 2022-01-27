#include "contour_buildup.hpp"
#include "molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "view/d3/spherical_triangle.hpp"
#include "view/d3/torus.hpp"
#include "worker/bitonic_sorter.hpp"

namespace VTX
{
	namespace Model
	{
		ContourBuildup::ContourBuildup( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_CONTOUR_BUILDUP ), _molecule( p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating contour buildup structure..." );

			{
				// The following code is based on the Megamol implementation
				// https://github.com/UniStuttgart-VISUS/megamol/blob/master/plugins/protein_cuda/src/MoleculeCBCudaRenderer.cpp#L901
				const std::vector<Atom *> &			 atoms = _molecule->getAtoms();
				const Molecule::AtomPositionsFrame & positions
					= _molecule->getAtomPositionFrame( _molecule->getFrame() );
				const uint paddedSize = Util::Math::nextPowerOfTwoValue( uint( atoms.size() ) );

				/*
				std::unique_ptr<GpuBuffer> ssboAtomPosition;
				_atomsInformation = std::vector<AtomInformation>();
				{
					const auto &			   atoms		  = _molecule->getAtoms();
					const std::vector<Vec3f> & atomsPositions = _molecule->getAtomPositions();
					_atomsInformation.reserve( _molecule->getAtomCount() );
					for ( std::size_t i = 0; i < atoms.size(); i++ )
					{
						const auto & currentPosition = atomsPositions[ i ];
						const Vec3i	 gridPos		 = _accelerationGrid.gridPosition( currentPosition );
						const uint	 hash			 = _accelerationGrid.gridHash( gridPos );

						_atomsInformation.emplace_back(
							AtomInformation { currentPosition, atoms[ i ]->getVdwRadius(), 0, 0, hash } );
					}

					for ( std::size_t i = atoms.size(); i < paddedSize; i++ )
					{
						_atomsInformation.emplace_back(
							AtomInformation { Vec3f( 0.f ), 0, 0, 0, std::numeric_limits<VTX::uint>::max() } );
					}
				}

				{
					std::vector<uint8_t> atomPositionData = std::vector<uint8_t>(
						sizeof( uint ) + 12 + _atomsInformation.size() * sizeof( AtomInformation ) );
					const uint atomNb = static_cast<uint>( atoms.size() );
					std::memcpy( atomPositionData.data(), &atomNb, sizeof( uint ) );
					std::memcpy( atomPositionData.data() + sizeof( uint ) + 12,
								 _atomsInformation.data(),
								 _atomsInformation.size() * sizeof( AtomInformation ) );
					ssboAtomPosition = std::make_unique<GpuBuffer>( atomPositionData );
				}

				std::unique_ptr<GpuBuffer> ssboCircles;
				auto buildGrid = std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/build_atoms_grid.comp" );
				auto countNeighbors = std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/count_neighbors.comp" );

				ssboAtomPosition->bind( 0 );
				bitonicSortFunction( "contourbuildup/sort_atoms.comp", paddedSize );
				ssboAtomPosition->unbind();

				auto atomGridIndices
					= std::vector<GridCellInfo>( static_cast<std::size_t>( _accelerationGrid.size.x )
													 * _accelerationGrid.size.y * _accelerationGrid.size.z,
												 GridCellInfo { -1, -1 } );
				auto ssboAtomCellInfo = std::make_unique<GpuBuffer>( atomGridIndices );

				ssboAtomPosition->bind( 0 );
				ssboAtomCellInfo->bind( 2 );
				buildGrid->start( _atomsInformation.size(), { GL_SHADER_STORAGE_BARRIER_BIT }, forcedFinish );
				ssboAtomCellInfo->unbind();
				ssboAtomPosition->unbind();

				const std::size_t circleMaximumNb = atoms.size() * MaxAtomNeighborNb;
				std::size_t		  circlePreAllocationSize
					= sizeof( uint ) +  12 + sizeof( CircleIntersection ) * circleMaximumNb;
				ssboCircles = std::make_unique<GpuBuffer>(
					circlePreAllocationSize, Renderer::BufferType::SSBO, Renderer::BufferAuthorization::READ );

				countNeighbors->getUniformManager().addValue( "uMaxNeighborsNb",
															  static_cast<int>( MaxAtomNeighborNb ) );
				countNeighbors->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				countNeighbors->getUniformManager().addValue( "uGridSize", _accelerationGrid.size );
				countNeighbors->getUniformManager().addValue( "uCellSize", _accelerationGrid.cellSize );
				countNeighbors->getUniformManager().addValue( "uWorldOrigin", _accelerationGrid.worldOrigin );
				const float maxVdwRadius = *std::max_element(
					Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
				countNeighbors->getUniformManager().addValue( "uMaxVdwRadius", maxVdwRadius );

				ssboAtomPosition->bind( 0 );
				ssboCircles->bind( 1 );
				ssboAtomCellInfo->bind( 2 );
				countNeighbors->start( atoms.size(), { GL_SHADER_STORAGE_BARRIER_BIT }, forcedFinish );
				ssboAtomPosition->unbind();
				ssboCircles->unbind();
				ssboAtomCellInfo->unbind();

				std::unique_ptr<GpuBuffer> ssboCirclesProbes;
				auto contourBuildUp = std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/contour_buildup.comp" );

				const uint * const realCircleNb
					= ssboCircles->map<uint>( 0, sizeof( uint ), Renderer::BufferAuthorization::READ );
				_torusPatchNb = static_cast<std::size_t>( *realCircleNb );
				ssboCircles->unmap();

				const std::size_t circleProbePreAllocationSize
					= sizeof( uint ) +  12 + sizeof( ProbePosition ) * _torusPatchNb * MaxProbePerCircle;

				ssboCirclesProbes = std::make_unique<GpuBuffer>(
					circleProbePreAllocationSize, Renderer::BufferType::SSBO, Renderer::BufferAuthorization::READ );

				contourBuildUp->getUniformManager().addValue( "uMaxNeighborsNb",
															  static_cast<GLint>( MaxAtomNeighborNb ) );
				contourBuildUp->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				contourBuildUp->getUniformManager().addValue( "uMaxProbePerCircle",
															  static_cast<GLint>( MaxProbePerCircle ) );

				ssboAtomPosition->bind( 0 );
				ssboCircles->bind( 1 );
				ssboCirclesProbes->bind( 2 );

				// Start a thread per circle
				contourBuildUp->start( _torusPatchNb, { GL_SHADER_STORAGE_BARRIER_BIT }, true );

				ssboAtomPosition->unbind();
				ssboCircles->unbind();
				ssboCirclesProbes->unbind();

				std::size_t paddedNbOfSpTriangle = 0;
				auto		buildPatch = std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/build_patch.comp" );

				const uint * const probeNb
					= ssboCirclesProbes->map<uint>( 0, sizeof( uint ), Renderer::BufferAuthorization::READ );
				_trianglePatchNb = static_cast<std::size_t>( *probeNb );
				ssboCirclesProbes->unmap();

				_ssboTorusPatches	 = std::make_unique<GpuBuffer>( sizeof( TorusPatch ) * _torusPatchNb );
				paddedNbOfSpTriangle = Util::Math::nextPowerOfTwoValue( _trianglePatchNb );

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

				buildPatch->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				buildPatch->getUniformManager().addValue( "uGridSize", _accelerationGrid.size );
				buildPatch->getUniformManager().addValue( "uCellSize", _accelerationGrid.cellSize );
				buildPatch->getUniformManager().addValue( "uWorldOrigin", _accelerationGrid.worldOrigin );

				ssboAtomPosition->bind( 0 );
				ssboCircles->bind( 1 );
				ssboCirclesProbes->bind( 2 );
				_ssboTorusPatches->bind( 3 );
				_ssboSpTrianglePatches->bind( 4 );

				// Start a thread per circle
				buildPatch->start( _torusPatchNb, { GL_SHADER_STORAGE_BARRIER_BIT }, forcedFinish );

				ssboAtomPosition->unbind();
				ssboCircles->unbind();
				ssboCirclesProbes->unbind();
				_ssboTorusPatches->unbind();
				_ssboSpTrianglePatches->unbind();

				// We no longer needs those buffers
				ssboAtomPosition.reset();
				ssboCircles.reset();
				ssboCirclesProbes.reset();
				auto buildProbesGrid
					= std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/build_probes_grid.comp" );
				auto buildSingularityBuffer
					= std::make_unique<VTX::Worker::GpuWorker>( "contourbuildup/build_singularity_buffer.comp" );

				_ssboSpTrianglePatches->bind( 0 );
				bitonicSortFunction( "contourbuildup/sort_probes.comp", paddedNbOfSpTriangle, 256 );
				_ssboSpTrianglePatches->unbind();

				auto probesGridIndices
					= std::vector<GridCellInfo>( static_cast<std::size_t>( _accelerationGrid.size.x )
													 * _accelerationGrid.size.y * _accelerationGrid.size.z,
												 GridCellInfo { -1, -1 } );
				auto ssboProbesCellInfo = std::make_unique<GpuBuffer>( probesGridIndices );

				_ssboSpTrianglePatches->bind( 0 );
				ssboProbesCellInfo->bind( 2 );
				buildProbesGrid->start( _trianglePatchNb, { GL_SHADER_STORAGE_BARRIER_BIT }, true );
				ssboProbesCellInfo->unbind();
				_ssboSpTrianglePatches->unbind();

				_ssboProbeIntersection
					= std::make_unique<GpuBuffer>( sizeof( Vec4f ) * _trianglePatchNb * MaxProbeNeighborNb );

				// Fill intersection texture
				buildSingularityBuffer->getUniformManager().addValue( "uProbeRadius", _probeRadius );
				buildSingularityBuffer->getUniformManager().addValue( "uMaxProbeNeighborNb",
																	  static_cast<int>( MaxProbeNeighborNb ) );
				buildSingularityBuffer->getUniformManager().addValue( "uGridSize", _accelerationGrid.size );
				buildSingularityBuffer->getUniformManager().addValue( "uCellSize", _accelerationGrid.cellSize );
				buildSingularityBuffer->getUniformManager().addValue( "uWorldOrigin", _accelerationGrid.worldOrigin );

				_ssboSpTrianglePatches->bind( 0 );
				_ssboProbeIntersection->bind( 1 );
				ssboProbesCellInfo->bind( 2 );

				// Start a thread per circle
				buildSingularityBuffer->start( _trianglePatchNb, { GL_SHADER_STORAGE_BARRIER_BIT }, forcedFinish );

				_ssboSpTrianglePatches->unbind();
				_ssboProbeIntersection->unbind();
				ssboProbesCellInfo->unbind();
				*/
			}

			chrono.stop();
			VTX_INFO( "Contour buildup structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

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
