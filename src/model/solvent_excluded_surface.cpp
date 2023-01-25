#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "category.hpp"
#include "chain.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/buffer.hpp"
#include "residue.hpp"
#include "selection/selection_manager.hpp"
#include "view/d3/triangle.hpp"
#include "worker/gpu_computer.hpp"

namespace VTX
{
	namespace Model
	{

		SolventExcludedSurface::SolventExcludedSurface( const Category * const p_category ) :
			MeshTriangle(), _category( p_category )
		{
		}

		const Math::Transform & SolventExcludedSurface::getTransform() const
		{
			return _category->getMoleculePtr()->getTransform();
		}

		void SolventExcludedSurface::_init()
		{
			refresh();
			refreshSelection( VTX::Selection::SelectionManager::get().getSelectionModel().getMoleculeMap(
				*_category->getMoleculePtr() ) );
		}

		void SolventExcludedSurface::refresh()
		{
			_mode = Mode::GPU;

			if ( _category->isEmpty() )
			{
				return;
			}

			switch ( _mode )
			{
			case SolventExcludedSurface::Mode::CPU: _refreshCPU(); break;
			case SolventExcludedSurface::Mode::GPU:
			{
				_buffer->makeContextCurrent();
				_refreshGPU();
				_buffer->doneContextCurrent();
				break;
			}
			default: break;
			}
		}

		void SolventExcludedSurface::_refreshGPU()
		{
			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_INFO( "Creating SES..." );

			const std::vector<uint> atomsIdx = _category->generateAtomIndexList();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _category->getMoleculePtr()->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize	 = gridMax - gridMin;
			Vec3i		atomGridSize = Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms
				= Object3D::Helper::Grid( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			std::vector<std::vector<uint>> atomGridDataTmp
				= std::vector<std::vector<uint>>( gridAtoms.getCellCount(), std::vector<uint>() );

			const std::vector<Vec3f> & atomPositions = _category->getMoleculePtr()->getCurrentAtomPositionFrame();

			// Store atom indices in acceleration grid.
			std::vector<Vec4f> atomPositionsVdW = std::vector<Vec4f>( atomPositions.size() );
			for ( uint idx : atomsIdx )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ idx ] );

				if ( hash >= atomGridDataTmp.size() )
				{
					continue;
				}

				atomGridDataTmp[ hash ].emplace_back( idx );
				atomPositionsVdW[ idx ]
					= Vec4f( atomPositions[ idx ], _category->getMoleculePtr()->getAtom( idx )->getVdwRadius() );
			}

			// Linerize data in 1D arrays.
			std::vector<Range> atomGridDataSorted = std::vector<Range>( gridAtoms.getCellCount(), Range { 0, 0 } );
			std::vector<uint>  atomIndexSorted	  = std::vector<uint>();

			for ( uint i = 0; i < atomGridDataTmp.size(); ++i )
			{
				const std::vector<uint> & data = atomGridDataTmp[ i ];
				if ( data.empty() == false )
				{
					atomGridDataSorted[ i ] = Range { uint( atomIndexSorted.size() ), uint( data.size() ) };
					atomIndexSorted.insert( atomIndexSorted.end(), data.begin(), data.end() );
				}
			}

			// Free tmp memory.
			atomGridDataTmp.clear();
			atomGridDataTmp.shrink_to_fit();

			chrono2.stop();
			VTX_INFO( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i				   sesGridSize = Vec3i( Util::Math::ceil( gridSize / VOXEL_SIZE ) );
			Object3D::Helper::Grid gridSES	   = Object3D::Helper::Grid( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			/////////////////////
			// Worker: create SDF.
			Worker::GpuComputer workerCreateSDF( IO::FilePath( "ses/create_sdf.comp" ) );

			// Create SSBOs.
			using VTX::Renderer::GL::Buffer;
			// Output.
			VTX_INFO( "size of {}", sizeof( SESGridData ) );
			Buffer bufferSesGridData( gridSES.getCellCount() * sizeof( SESGridData ) );
			// Input.
			Buffer bufferAtomGridDataSorted( atomGridDataSorted );
			Buffer bufferAtomIndexSorted( atomIndexSorted );
			Buffer bufferAtomPosition( atomPositionsVdW );

			// Bind.
			bufferSesGridData.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferAtomGridDataSorted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferAtomIndexSorted.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomPosition.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );

			// Set uniforms.
			workerCreateSDF.getProgram().use();

			workerCreateSDF.getProgram().setVec3f( "uGridAtomWorldOrigin", gridAtoms.worldOrigin );
			workerCreateSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerCreateSDF.getProgram().setVec3u( "uGridAtomSize", Vec3u( gridAtoms.size ) );
			workerCreateSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerCreateSDF.getProgram().setVec3f( "uGridAtomCellSize", gridAtoms.cellSize );
			workerCreateSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerCreateSDF.getProgram().setUInt( "uGridAtomCellCount", uint( gridAtoms.getCellCount() ) );
			workerCreateSDF.getProgram().setUInt( "uGridSESCellCount", uint( gridSES.getCellCount() ) );
			workerCreateSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );
			workerCreateSDF.getProgram().setFloat( "uVoxelSize", VOXEL_SIZE );

			// Start.
			if ( LOCAL_SIZE_X == 1 )
			{
				workerCreateSDF.start( gridSES.size );
			}
			else
			{
				workerCreateSDF.start( gridSES.getCellCount() );
			}

			// Unbind.
			bufferAtomGridDataSorted.unbind();
			bufferAtomIndexSorted.unbind();
			bufferAtomPosition.unbind();

			chrono2.stop();
			VTX_INFO( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			//////////////////////
			// Worker: refine SDF.
			Worker::GpuComputer workerRefineSDF( IO::FilePath( "ses/refine_sdf.comp" ) );
			workerRefineSDF.getProgram().use();

			Vec3i cellsToVisitCount = Util::Math::ceil( Vec3f( PROBE_RADIUS + VOXEL_SIZE ) / gridSES.cellSize );

			workerRefineSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerRefineSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerRefineSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerRefineSDF.getProgram().setUInt( "uGridSESCellCount", uint( gridSES.getCellCount() ) );
			workerRefineSDF.getProgram().setVec3i( "uCellsToVisitCount", cellsToVisitCount );
			workerRefineSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );

			// Start
			if ( LOCAL_SIZE_X == 1 )
			{
				workerRefineSDF.start( gridSES.size );
			}
			else
			{
				workerRefineSDF.start( gridSES.getCellCount() );
			}

			chrono2.stop();
			VTX_INFO( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			/////////////////////////
			// Worker: marching cube.
			Worker::GpuComputer workerMarchingCube( IO::FilePath( "ses/marching_cube.comp" ) );

			// Create SSBOs.
			// Output.
			// 5 triangles max per cell.
			const size_t	  bufferSize = gridSES.getCellCount() * 5 * 3;
			Buffer			  bufferPositionsTmp( bufferSize * sizeof( Vec4f ) );
			Buffer			  bufferNormalsTmp( bufferSize * sizeof( Vec4f ) );
			Buffer			  bufferAtomIdsTmp( bufferSize * sizeof( uint ) );
			std::vector<uint> triangleValidities( bufferSize, 0 );
			Buffer			  bufferTriangleValidities( triangleValidities );
			// Input.
			Buffer bufferTriangleTable( 256 * 16 * sizeof( int ), Math::MarchingCube::TRIANGLE_TABLE );
			Buffer bufferAtomIds( _category->getMoleculePtr()->getBufferAtomIds() );

			bufferPositionsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferNormalsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferAtomIdsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferTriangleValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferTriangleTable.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );
			bufferAtomIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 6 );

			workerMarchingCube.getProgram().use();

			workerMarchingCube.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerMarchingCube.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerMarchingCube.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );

			// Start.
			if ( LOCAL_SIZE_X == 1 )
			{
				workerMarchingCube.start( gridSES.size );
			}
			else
			{
				workerMarchingCube.start( gridSES.getCellCount() );
			}

			// Get validities for next step.
			bufferTriangleValidities.getData( triangleValidities );

			// Unbind.
			bufferSesGridData.unbind();
			bufferPositionsTmp.unbind();
			bufferNormalsTmp.unbind();
			bufferAtomIdsTmp.unbind();
			bufferTriangleValidities.unbind();
			bufferTriangleTable.unbind();
			bufferAtomIds.unbind();

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			////////////////////////////
			// Worker: stream compaction.
			Worker::GpuComputer workerStreamCompaction( IO::FilePath( "ses/stream_compaction.comp" ) );
			VTX_DEBUG( "Triangle buffer size before compaction: {}", bufferSize );
			//  Perform exclusive scan on validity buffer.
			std::exclusive_scan( triangleValidities.begin(), triangleValidities.end(), triangleValidities.begin(), 0 );

			size_t bufferSizeReduced = *( triangleValidities.end() - 1 );
			_indiceCount			 = uint( bufferSizeReduced );

			VTX_DEBUG( "Triangle buffer size after compaction: {}", _indiceCount );

			// Create SSBOs.
			// Output.
			Buffer & bufferPositions	= _buffer->getBufferPositions();
			Buffer & bufferNormals		= _buffer->getBufferNormals();
			Buffer & bufferIndices		= _buffer->getBufferIndices();
			Buffer & bufferColors		= _buffer->getBufferColors();
			Buffer & bufferVisibilities = _buffer->getBufferVisibilities();
			Buffer & bufferIds			= _buffer->getBufferIds();
			Buffer & bufferSelections	= _buffer->getBufferSelections();

			if ( _isInit == false )
			{
				bufferPositions.set( bufferSizeReduced * sizeof( Vec4f ) );
				bufferNormals.set( bufferSizeReduced * sizeof( Vec4f ) );
				bufferIndices.set( bufferSizeReduced * sizeof( uint ) );
				bufferColors.set( bufferSizeReduced * sizeof( Color::Rgba ), Buffer::Flags::MAP_WRITE_BIT );
				bufferVisibilities.set( bufferSizeReduced * sizeof( uint ), Buffer::Flags::DYNAMIC_STORAGE_BIT );
				bufferIds.set( bufferSizeReduced * sizeof( uint ) );
				bufferSelections.set( bufferSizeReduced * sizeof( uint ), Buffer::Flags::DYNAMIC_STORAGE_BIT );
			}

			// Input.
			Buffer bufferTriangleValiditiesSum( triangleValidities );
			Buffer bufferAtomColors( _category->getMoleculePtr()->getBufferAtomColors() );
			Buffer bufferAtomVisibilities( _category->getMoleculePtr()->getBufferAtomVisibilities() );

			// Bind.
			bufferPositions.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 0 );
			bufferNormals.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 1 );
			bufferIndices.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 2 );
			bufferColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 3 );
			bufferVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 4 );
			bufferIds.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 5 );

			bufferPositionsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 6 );
			bufferNormalsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 7 );
			bufferAtomIdsTmp.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 8 );
			bufferTriangleValidities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 9 );
			bufferTriangleValiditiesSum.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 10 );
			bufferAtomColors.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 11 );
			bufferAtomVisibilities.bind( Buffer::Target::SHADER_STORAGE_BUFFER, 12 );

			workerStreamCompaction.getProgram().use();
			workerStreamCompaction.getProgram().setUInt( "uSize", uint( bufferSize ) );
			workerStreamCompaction.getProgram().setUInt( "uSizeReduced", uint( bufferSizeReduced ) );

			// Start.
			workerStreamCompaction.start( bufferSize );

			// Unbind.
			bufferPositions.unbind();
			bufferNormals.unbind();
			bufferIndices.unbind();
			bufferColors.unbind();
			bufferVisibilities.unbind();
			bufferIds.unbind();

			bufferPositionsTmp.unbind();
			bufferNormalsTmp.unbind();
			bufferAtomIdsTmp.unbind();
			bufferTriangleValiditiesSum.unbind();
			bufferAtomColors.unbind();
			bufferAtomVisibilities.unbind();

			/////////// TMP.
			_atomsToTriangles				   = std::vector<Range>( atomPositions.size(), Range { 0, 0 } );
			_atomsToTriangles[ atomsIdx[ 0 ] ] = Range { 0, _indiceCount };

			_atomsToTriangles.shrink_to_fit();

			chrono2.stop();
			VTX_INFO( "Buffer filled in " + std::to_string( chrono2.elapsedTime() ) + "s" );

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::_refreshCPU()
		{
			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_INFO( "Creating SES..." );

			_vertices.clear();
			_normals.clear();
			_indices.clear();
			_ids.clear();
			_colors.clear();
			_visibilities.clear();
			_atomsToTriangles.clear();

			const std::vector<uint> atomsIdx = _category->generateAtomIndexList();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _category->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize = gridMax - gridMin;
			Vec3i		atomGridSize( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			const std::vector<Vec3f> & atomPositions = _category->getMoleculePtr()->getCurrentAtomPositionFrame();

			std::vector<std::vector<uint>> atomGridData2D( gridAtoms.getCellCount(), std::vector<uint>() );

			// Store atom indices in acceleration grid.
			for ( uint i : atomsIdx )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ i ] );

				if ( hash < atomGridData2D.size() )
				{
					atomGridData2D[ hash ].emplace_back( i );
				}
			}

			chrono2.stop();
			VTX_INFO( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i sesGridSize( Util::Math::ceil( gridSize / VOXEL_SIZE ) );

			Object3D::Helper::Grid gridSES( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			// SES grid data.
			std::vector<SESGridData> sesGridData( gridSES.getCellCount(), SESGridData { PROBE_RADIUS, -1 } );

			// Loop over cells.
			std::vector<uint> boundaryIndex = std::vector<uint>();
			for ( uint sesGridHash = 0; sesGridHash < gridSES.getCellCount(); ++sesGridHash )
			{
				// Get corresponding ses grid data.
				const Vec3i	  sesGridPosition = gridSES.gridPosition( sesGridHash );
				SESGridData & gridData		  = sesGridData[ sesGridHash ];

				// Get corresponding acceleration grid cell hash.
				const Vec3f sesGridCellWorldPosition = gridSES.worldPosition( sesGridPosition );
				const Vec3i atomGridPosition		 = gridAtoms.gridPosition( sesGridCellWorldPosition );

				// Loop over the 27 cells to visit.
				float minDistance = FLOAT_MAX;
				bool  found		  = false;

				for ( int ox = -1; ox <= 1 && !found; ++ox )
				{
					for ( int oy = -1; oy <= 1 && !found; ++oy )
					{
						for ( int oz = -1; oz <= 1 && !found; ++oz )
						{
							Vec3f offset( ox, oy, oz );
							Vec3i gridPositionToVisit( Vec3f( atomGridPosition ) + offset );
							uint  hashToVisit = gridAtoms.gridHash( Vec3i( gridPositionToVisit ) );

							if ( hashToVisit >= atomGridData2D.size() )
							{
								continue;
							}

							// Compute SDF.
							for ( const uint index : atomGridData2D[ hashToVisit ] )
							{
								if ( _category->getMoleculePtr()->getAtom( index ) == nullptr )
								{
									continue;
								}

								float distance
									= Util::Math::distance( atomPositions[ index ], sesGridCellWorldPosition );

								// Inside.
								if ( distance < VOXEL_SIZE )
								{
									gridData.sdf		 = -VOXEL_SIZE;
									gridData.nearestAtom = -1;
									found				 = true;
									// Don't need to loop over other cells.
									break;
								}
								// Boundary.
								else
								{
									distance -= ( PROBE_RADIUS
												  + _category->getMoleculePtr()->getAtom( index )->getVdwRadius() );
									if ( distance < 0.f )
									{
										gridData.sdf = -VOXEL_SIZE;
										if ( distance < minDistance )
										{
											minDistance			 = distance;
											gridData.nearestAtom = index;
										}
									}
								}
							}
						}
					}
				}
			}

			chrono2.stop();
			VTX_INFO( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );

			chrono2.start();

			// SDF refinement.
			Vec3i cellsToVisitCount = Util::Math::ceil( Vec3f( PROBE_RADIUS + VOXEL_SIZE ) / gridSES.cellSize );

			// for ( uint sesGridHash : boundaryIndex ) // Why this is slower?
			for ( uint sesGridHash = 0; sesGridHash < gridSES.getCellCount(); ++sesGridHash )
			{
				// Get corresponding ses grid data.
				const Vec3i	  sesGridPosition = gridSES.gridPosition( sesGridHash );
				SESGridData & gridData		  = sesGridData[ sesGridHash ];

				// Not in boundary.
				if ( gridData.nearestAtom == -1 )
				{
					continue;
				}

				const Vec3f sesWorldPosition = gridSES.worldPosition( sesGridPosition );

				float minDistanceWithOutsidePoint = FLOAT_MAX;
				bool  found						  = false;
				for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x; ++ox )
				{
					for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y; ++oy )
					{
						for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z; ++oz )
						{
							Vec3f		offset				= Vec3f( ox, oy, oz );
							const Vec3i gridPositionToVisit = Vec3i( Vec3f( sesGridPosition ) + offset );
							const uint	hashToVisit			= gridSES.gridHash( gridPositionToVisit );

							if ( hashToVisit >= sesGridData.size() )
							{
								continue;
							}

							const Vec3f	  worldPositionToVisit = gridSES.worldPosition( gridPositionToVisit );
							SESGridData & gridDataToVisit	   = sesGridData[ hashToVisit ];

							// If outside.
							if ( gridDataToVisit.sdf == PROBE_RADIUS )
							{
								const float distance = Util::Math::distance( worldPositionToVisit, sesWorldPosition );
								if ( distance < minDistanceWithOutsidePoint )
								{
									minDistanceWithOutsidePoint = distance;
								}
								found = true;
							}
						}
					}
				}

				if ( found )
				{
					gridData.sdf = PROBE_RADIUS - minDistanceWithOutsidePoint;
				}
			}

			chrono2.stop();

			VTX_INFO( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			std::vector<std::vector<Vec4f>> atomsToTriangles( atomPositions.size(), std::vector<Vec4f>() );

			// Marching cube to extract mesh.
			const Math::MarchingCube marchingCube = Math::MarchingCube();
			for ( uint x = 0; x < uint( gridSES.size.x ) - 1; ++x )
			{
				for ( uint y = 0; y < uint( gridSES.size.y ) - 1; ++y )
				{
					for ( uint z = 0; z < uint( gridSES.size.z ) - 1; ++z )
					{
						SESGridData gridData[ 8 ] = { sesGridData[ gridSES.gridHash( Vec3i( x, y, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y + 1, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y + 1, z ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x + 1, y + 1, z + 1 ) ) ],
													  sesGridData[ gridSES.gridHash( Vec3i( x, y + 1, z + 1 ) ) ] };

						Math::MarchingCube::GridCell cell
							= { { gridSES.worldPosition( Vec3i( x, y, z ) ),
								  { gridSES.worldPosition( Vec3i( x + 1, y, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y + 1, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y + 1, z ) ) },
								  { gridSES.worldPosition( Vec3i( x + 1, y + 1, z + 1 ) ) },
								  { gridSES.worldPosition( Vec3i( x, y + 1, z + 1 ) ) } },
								{ gridData[ 0 ].sdf,
								  gridData[ 1 ].sdf,
								  gridData[ 2 ].sdf,
								  gridData[ 3 ].sdf,
								  gridData[ 4 ].sdf,
								  gridData[ 5 ].sdf,
								  gridData[ 6 ].sdf,
								  gridData[ 7 ].sdf } };

						std::vector<std::vector<Vec4f>> cellTriangles = marchingCube.triangulateCell( cell, 0 );
						for ( std::vector<Vec4f> & cellTriangle : cellTriangles )
						{
							assert( cellTriangle.size() == 3 );

							// Get closest atom.
							float closestDistance = FLOAT_MAX;
							uint  closestVertex	  = 0;
							Vec3f centroid = Vec3f( cellTriangle[ 0 ] + cellTriangle[ 1 ] + cellTriangle[ 2 ] ) / 3.f;
							for ( uint vertex = 0; vertex < 8; ++vertex )
							{
								float distance = Util::Math::distance( centroid, cell.vertex[ vertex ] );
								if ( distance < closestDistance )
								{
									closestVertex	= vertex;
									closestDistance = distance;
								}
							}

							// Map atoms with triangle points.
							std::vector<Vec4f> & triangles = atomsToTriangles[ gridData[ closestVertex ].nearestAtom ];
							triangles.insert( triangles.end(), cellTriangle.begin(), cellTriangle.end() );
						}
					}
				}
			}

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Fill buffers with sorted values and store data as triangle range per atoms.
			_atomsToTriangles = std::vector<Range>( atomPositions.size(), Range { 0, 0 } );
			for ( uint i = 0; i < atomsToTriangles.size(); ++i )
			{
				if ( _category->getMoleculePtr()->getAtom( i ) == nullptr )
				{
					continue;
				}

				const std::vector<Vec4f> & trianglePoints = atomsToTriangles[ i ];

				_atomsToTriangles[ i ].first = uint( _vertices.size() );
				_atomsToTriangles[ i ].count = uint( trianglePoints.size() );

				std::vector<uint> indices = std::vector<uint>( trianglePoints.size() );
				int				  index	  = int( _vertices.size() );
				std::iota( indices.begin(), indices.end(), index );
				_indices.insert( _indices.end(), indices.begin(), indices.end() );

				_vertices.insert( _vertices.end(), trianglePoints.begin(), trianglePoints.end() );

				std::vector<uint> ids = std::vector<uint>( trianglePoints.size() );
				std::fill( ids.begin(), ids.end(), _category->getMoleculePtr()->getAtom( i )->getId() );
				_ids.insert( _ids.end(), ids.begin(), ids.end() );
			}

			chrono2.stop();
			VTX_INFO( "Triangles sorting done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();
			// toIndexed();
			chrono2.stop();
			VTX_INFO( "Mesh to indexed computed in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();
			recomputeNormals();
			chrono2.stop();
			VTX_INFO( "Normals computed in " + std::to_string( chrono2.elapsedTime() ) + "s" );

			_indiceCount = uint( _indices.size() );

			refreshColors();
			refreshVisibilities();

			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setIds( _ids );
			_buffer->setIndices( _indices );

			_vertices.clear();
			_normals.clear();
			_ids.clear();
			_indices.clear();

			_vertices.shrink_to_fit();
			_normals.shrink_to_fit();
			_ids.shrink_to_fit();
			_indices.shrink_to_fit();

			_atomsToTriangles.shrink_to_fit();

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		// TODO: check if it is still needed after creation.
		void SolventExcludedSurface::refreshColors()
		{
			if ( _mode == Mode::CPU )
			{
				_colors.resize( _indiceCount, Color::Rgba::WHITE );

				for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
				{
					const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
					if ( atom == nullptr )
					{
						continue;
					}

					const Color::Rgba & color = _category->getMoleculePtr()->getAtomColor( atomIdx );
					std::fill(
						_colors.begin() + _atomsToTriangles[ atomIdx ].first,
						_colors.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						color );
				}
				_buffer->setColors( _colors );
				_colors.clear();
				_colors.shrink_to_fit();
			}
			else
			{
				using VTX::Renderer::GL::Buffer;
				_buffer->makeContextCurrent();
				const Buffer & bufferColor = _buffer->getBufferColors();

				Color::Rgba * const ptr = bufferColor.map<Color::Rgba>( Buffer::Access::WRITE_ONLY );

				for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
				{
					const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
					if ( atom == nullptr )
					{
						continue;
					}

					const Color::Rgba & color = _category->getMoleculePtr()->getAtomColor( atomIdx );

					// TODO: optimize that.
					for ( uint i = 0; i < _atomsToTriangles[ atomIdx ].count; ++i )
					{
						ptr[ _atomsToTriangles[ atomIdx ].first + i ] = color;
					}
				}

				bufferColor.unmap();
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			using VTX::Renderer::GL::Buffer;

			if ( _mode == Mode::GPU )
			{
				_buffer->makeContextCurrent();
			}

			_visibilities.resize( _indiceCount, 1 );
			const Buffer & bufferVisibilities = _buffer->getBufferVisibilities();

			std::vector<uint> & visitibilities = _category->getMoleculePtr()->getBufferAtomVisibilities();

			for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
			{
				const Atom * const atom = _category->getMoleculePtr()->getAtom( atomIdx );
				if ( atom == nullptr )
				{
					continue;
				}

				if ( visitibilities[ atomIdx ] == 0 )
				{
					std::fill(
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first,
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						0 );
				}
			}

			if ( _mode == Mode::CPU )
			{
				_buffer->setVisibilities( _visibilities );
			}
			else
			{
				bufferVisibilities.setSub( _visibilities );
			}

			_visibilities.clear();
			_visibilities.shrink_to_fit();

			if ( _mode == Mode::GPU )
			{
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
		{
			using VTX::Renderer::GL::Buffer;

			if ( _mode == Mode::GPU )
			{
				_buffer->makeContextCurrent();
			}

			_selections.resize( _indiceCount, 0 );
			const Buffer & bufferSelections = _buffer->getBufferSelections();

			if ( p_selection != nullptr )
			{
				if ( p_selection->getFullySelectedChildCount() == _category->getMoleculePtr()->getRealChainCount() )
				{
					std::fill( _selections.begin(), _selections.end(), 1 );
				}
				else
				{
					for ( const Model::Selection::PairChainIds & pairChain : *p_selection )
					{
						for ( const Model::Selection::PairResidueIds & pairResidue : pairChain.second )
						{
							for ( const uint & atomIdx : pairResidue.second )
							{
								std::fill( _selections.begin() + _atomsToTriangles[ atomIdx ].first,
										   _selections.begin() + _atomsToTriangles[ atomIdx ].first
											   + _atomsToTriangles[ atomIdx ].count,
										   1 );
							}
						}
					}
				}
			}

			if ( _mode == Mode::CPU )
			{
				_buffer->setSelections( _selections );
			}
			else
			{
				bufferSelections.setSub( _selections );
			}

			_selections.clear();
			_selections.shrink_to_fit();

			if ( _mode == Mode::GPU )
			{
				_buffer->doneContextCurrent();
			}
		}

		void SolventExcludedSurface::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::TriangleSES>( this, VTX::ID::View::D3_TRIANGLE_SES ) );
		}
	} // namespace Model
} // namespace VTX
