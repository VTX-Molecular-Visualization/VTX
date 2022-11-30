#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/buffer_storage.hpp"
#include "residue.hpp"
#include "selection/selection_manager.hpp"
#include "view/d3/triangle.hpp"
#include "worker/gpu_computer.hpp"

namespace VTX
{
	namespace Model
	{
		SolventExcludedSurface::SolventExcludedSurface( Molecule * const p_molecule ) :
			MeshTriangle(), _molecule( p_molecule )
		{
		}

		const Math::Transform & SolventExcludedSurface::getTransform() const { return _molecule->getTransform(); }

		void SolventExcludedSurface::_init()
		{
			refresh();
			refreshSelection(
				VTX::Selection::SelectionManager::get().getSelectionModel().getMoleculeMap( *_molecule ) );
		}

		void SolventExcludedSurface::refresh()
		{
			_mode = Mode::CPU;

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

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _molecule->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize	 = gridMax - gridMin;
			Vec3i		atomGridSize = Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms
				= Object3D::Helper::Grid( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			std::vector<std::vector<uint>> atomGridDataTmp
				= std::vector<std::vector<uint>>( gridAtoms.getCellCount(), std::vector<uint>() );

			const std::vector<Vec3f> & atomPositions = _molecule->getCurrentAtomPositionFrame();

			// Store atom indices in acceleration grid.
			// TODO: remove this loop and create directly 1D arrays?
			// vec4( position.xyz, vdwRadius )
			std::vector<Vec4f> atomPositionsVdW = std::vector<Vec4f>( atomPositions.size() );
			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ i ] );
				atomGridDataTmp[ hash ].emplace_back( i );
				atomPositionsVdW[ i ] = Vec4f( atomPositions[ i ], _molecule->getAtom( i )->getVdwRadius() );
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

			chrono2.stop();
			VTX_INFO( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i				   sesGridSize = Vec3i( Util::Math::ceil( gridSize / VOXEL_SIZE ) );
			Object3D::Helper::Grid gridSES	   = Object3D::Helper::Grid( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			// SES grid data.
			std::vector<SESGridData> sesGridData
				= std::vector<SESGridData>( gridSES.getCellCount(), SESGridData { PROBE_RADIUS, -1 } );

			// Worker: create SDF.
			Worker::GpuComputer workerCreateSDF( IO::FilePath( "ses/create_sdf.comp" ) );

			// Create SSBOs.
			using VTX::Renderer::GL::BufferStorage;
			// // Output.
			BufferStorage ssboSesGridData( VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER,
										   sesGridData );
			// Input.

			const BufferStorage ssboAtomGridDataSorted( BufferStorage::Target::SHADER_STORAGE_BUFFER,
														atomGridDataSorted );
			const BufferStorage ssboAtomIndexSorted( BufferStorage::Target::SHADER_STORAGE_BUFFER, atomIndexSorted );
			const BufferStorage ssboAtomPosition( BufferStorage::Target::SHADER_STORAGE_BUFFER, atomPositionsVdW );
			// const BufferStorage ssboAtomVdwRadius( BufferStorage::Target::SHADER_STORAGE_BUFFER, atomVdwRadius );

			// TODO: clear CPU buffers?
			// Bind.
			ssboSesGridData.bind( 0 );
			ssboAtomGridDataSorted.bind( 1 );
			ssboAtomIndexSorted.bind( 2 );
			ssboAtomPosition.bind( 3 );
			// ssboAtomVdwRadius.bind( 4 );

			// Set uniforms.
			workerCreateSDF.getProgram().use();

			workerCreateSDF.getProgram().setVec3f( "uGridAtomWorldOrigin", gridAtoms.worldOrigin );
			workerCreateSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerCreateSDF.getProgram().setVec3u( "uGridAtomSize", Vec3u( gridAtoms.size ) );
			workerCreateSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerCreateSDF.getProgram().setVec3f( "uGridAtomCellSize", gridAtoms.cellSize );
			workerCreateSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerCreateSDF.getProgram().setUInt( "uGridAtomCellCount", gridAtoms.getCellCount() );
			workerCreateSDF.getProgram().setUInt( "uGridSESCellCount", gridSES.getCellCount() );
			workerCreateSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );
			workerCreateSDF.getProgram().setFloat( "uVoxelSize", VOXEL_SIZE );

			// Start.
			workerCreateSDF.start( gridSES.size );

			// Unbind.
			ssboAtomGridDataSorted.unbind();
			ssboAtomIndexSorted.unbind();
			ssboAtomPosition.unbind();
			// ssboAtomVdwRadius.unbind();

			chrono2.stop();
			VTX_INFO( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Worker: refine SDF.
			Worker::GpuComputer workerRefineSDF( IO::FilePath( "ses/refine_sdf.comp" ) );
			workerRefineSDF.getProgram().use();

			Vec3i cellsToVisitCount = Util::Math::ceil( Vec3f( PROBE_RADIUS + VOXEL_SIZE ) / gridSES.cellSize );

			workerRefineSDF.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerRefineSDF.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerRefineSDF.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			workerRefineSDF.getProgram().setUInt( "uGridSESCellCount", gridSES.getCellCount() );
			workerRefineSDF.getProgram().setVec3i( "uCellsToVisitCount", cellsToVisitCount );
			workerRefineSDF.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );

			// Start
			workerRefineSDF.start( gridSES.size );

			// ssboSesGridData.getData( 0, uint( sesGridData.size() ) * sizeof( SESGridData ), &sesGridData[ 0 ] );
			// std::ofstream outFile( "GPU_DATA.txt" );
			// for ( const auto & e : sesGridData )
			//	outFile << std::to_string( e.sdf ) + " " + std::to_string( e.nearestAtom ) << "\n";
			// outFile.close();

			chrono2.stop();
			VTX_INFO( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Worker: marching cube.
			Worker::GpuComputer workerMarchingCube( IO::FilePath( "ses/marching_cube.comp" ) );

			// Create SSBOs.
			// Output.
			_vertices		  = std::vector<Vec3f>( gridSES.getCellCount() * 5 * 3, VEC3F_ZERO );
			_indices		  = std::vector<uint>( _vertices.size(), 0 );
			_normals		  = std::vector<Vec3f>( _vertices.size(), VEC3F_ZERO );
			_ids			  = std::vector<uint>( _vertices.size(), 0 );
			_atomsToTriangles = std::vector<Range>( atomPositions.size(), Range { 0, 0 } );

			std::vector<Vec4f> vertices( gridSES.getCellCount() * 5 * 3, Vec4f() );
			std::vector<Vec4f> normals( _vertices.size(), Vec4f() );

			BufferStorage ssboTrianglePositions( BufferStorage::Target::SHADER_STORAGE_BUFFER, vertices );
			BufferStorage ssboTriangleIndices( BufferStorage::Target::SHADER_STORAGE_BUFFER, _indices );
			BufferStorage ssboTriangleNormals( BufferStorage::Target::SHADER_STORAGE_BUFFER, normals );
			BufferStorage ssboTriangleAtomIds( BufferStorage::Target::SHADER_STORAGE_BUFFER, _ids );
			BufferStorage ssboAtomToTriangles( BufferStorage::Target::SHADER_STORAGE_BUFFER, _atomsToTriangles );

			// Input.
			const BufferStorage ssboTriangleTable( VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER,
												   256 * 16 * sizeof( int ),
												   Math::MarchingCube::TRIANGLE_TABLE,
												   VTX::Renderer::GL::BufferStorage::Flags::DYNAMIC_STORAGE_BIT );

			ssboTrianglePositions.bind( 1 );
			ssboTriangleIndices.bind( 2 );
			ssboTriangleNormals.bind( 3 );
			ssboTriangleAtomIds.bind( 4 );
			ssboAtomToTriangles.bind( 5 );
			ssboTriangleTable.bind( 6 );

			workerMarchingCube.getProgram().use();

			workerMarchingCube.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			workerMarchingCube.getProgram().setVec3u( "uGridSESSize", Vec3u( gridSES.size ) );
			workerMarchingCube.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );

			// Start
			workerMarchingCube.start( gridSES.size );

			ssboTrianglePositions.getData( 0, uint( vertices.size() ) * sizeof( Vec3f ), &vertices[ 0 ] );
			ssboTriangleIndices.getData( 0, uint( _indices.size() ) * sizeof( uint ), &_indices[ 0 ] );
			ssboTriangleNormals.getData( 0, uint( normals.size() ) * sizeof( Vec3f ), &normals[ 0 ] );
			ssboTriangleAtomIds.getData( 0, uint( _ids.size() ) * sizeof( uint ), &_ids[ 0 ] );
			//  ssboAtomToTriangles.getData(
			//	0, uint( _atomsToTriangles.size() ) * sizeof( Range ), &_atomsToTriangles[ 0 ] );

			for ( uint i = 0; i < _vertices.size(); ++i )
			{
				_vertices[ i ] = Vec3f( vertices[ i ] );
				_normals[ i ]  = Vec3f( normals[ i ] );
			}

			//_vertices.emplace_back( vertices.begin(), vertices.end() );
			//_normals.emplace_back( normals.begin(), normals.end() );

			//////////////////////
			_atomsToTriangles[ _atomsToTriangles.size() - 1 ] = Range { 0, uint( _indices.size() ) };
			//////////////////////
			//
			//////////////////////
			// std::ofstream outFile( "GPU_DATA.txt" );
			//  for ( const auto & e : _vertices )
			//  outFile << std::to_string( e.x ) + " " + std::to_string( e.y ) << " " << std::to_string( e.z ) << "\n";
			//  for ( const auto & e : _indices )
			//	outFile << std::to_string( e ) << "\n";
			// outFile.close();
			//////////////////////

			// Unbind.
			ssboSesGridData.unbind();
			ssboTrianglePositions.unbind();
			ssboTriangleIndices.unbind();
			ssboTriangleNormals.unbind();
			ssboTriangleAtomIds.unbind();
			ssboAtomToTriangles.unbind();
			ssboTriangleTable.unbind();

			assert( _vertices.size() == _indices.size() );
			assert( _vertices.size() == _normals.size() );
			assert( _vertices.size() == _ids.size() );

			_indiceCount = uint( _indices.size() );

			// refreshColors();
			_colors.resize( _indiceCount, Color::Rgb::WHITE );
			// refreshVisibilities();
			_visibilities.resize( _indiceCount, 1 );

			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setColors( _colors );
			_buffer->setVisibilities( _visibilities );
			_buffer->setIds( _ids );
			_buffer->setIndices( _indices );

			_vertices.clear();
			_normals.clear();
			_colors.clear();
			_visibilities.clear();
			_ids.clear();
			_indices.clear();

			_vertices.shrink_to_fit();
			_normals.shrink_to_fit();
			_colors.shrink_to_fit();
			_visibilities.shrink_to_fit();
			_ids.shrink_to_fit();
			_indices.shrink_to_fit();

			_atomsToTriangles.shrink_to_fit();

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );

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
			_indices.clear();
			_ids.clear();

			// Sort atoms in acceleration grid.
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _molecule->getAABB();

			const float atomGridCellSize = PROBE_RADIUS + maxVdWRadius;
			const Vec3f gridMin			 = molAABB.getMin() - atomGridCellSize;
			const Vec3f gridMax			 = molAABB.getMax() + atomGridCellSize;

			const Vec3f gridSize	 = gridMax - gridMin;
			Vec3i		atomGridSize = Vec3i( Util::Math::ceil( gridSize / atomGridCellSize ) );

			Object3D::Helper::Grid gridAtoms
				= Object3D::Helper::Grid( gridMin, Vec3f( atomGridCellSize ), atomGridSize );

			const std::vector<Vec3f> & atomPositions = _molecule->getCurrentAtomPositionFrame();

			std::vector<std::vector<uint>> atomGridDataTmp
				= std::vector<std::vector<uint>>( gridAtoms.getCellCount(), std::vector<uint>() );

			// Store atom indices in acceleration grid.
			// TODO: remove this loop and create directly 1D arrays?
			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ i ] );
				atomGridDataTmp[ hash ].emplace_back( i );
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

			chrono2.stop();
			VTX_INFO( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			Vec3i sesGridSize = Vec3i( Util::Math::ceil( gridSize / VOXEL_SIZE ) );

			Object3D::Helper::Grid gridSES = Object3D::Helper::Grid( gridMin, Vec3f( VOXEL_SIZE ), sesGridSize );

			// SES grid data.
			std::vector<SESGridData> sesGridData
				= std::vector<SESGridData>( gridSES.getCellCount(), SESGridData { PROBE_RADIUS, -1 } );

			// Loop over cells.
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
							Vec3f offset			  = Vec3f( ox, oy, oz );
							Vec3i gridPositionToVisit = Vec3i( Vec3f( atomGridPosition ) + offset );
							uint  hashToVisit		  = gridAtoms.gridHash( Vec3i( gridPositionToVisit ) );

							if ( hashToVisit >= atomGridDataSorted.size() )
							{
								continue;
							}

							uint first = atomGridDataSorted[ hashToVisit ].first;
							uint count = atomGridDataSorted[ hashToVisit ].count;

							// Compute SDF.
							for ( uint i = first; i < first + count; ++i )

							{
								uint index = atomIndexSorted[ i ];
								if ( _molecule->getAtom( index ) == nullptr )
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
									distance -= ( PROBE_RADIUS + _molecule->getAtom( index )->getVdwRadius() );
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

			// std::ofstream outFile("CPU_DATA.txt");
			// for (const auto& e : sesGridData)
			//	outFile << std::to_string(e.sdf) + " " + std::to_string(e.nearestAtom) << "\n";
			// outFile.close();

			chrono2.stop();

			VTX_INFO( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			std::vector<std::vector<Vec3f>> atomsToTriangles
				= std::vector<std::vector<Vec3f>>( atomPositions.size(), std::vector<Vec3f>() );

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

						std::vector<std::vector<Vec3f>> cellTriangles = marchingCube.triangulateCell( cell, 0 );
						for ( std::vector<Vec3f> & cellTriangle : cellTriangles )
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
							std::vector<Vec3f> & triangles = atomsToTriangles[ gridData[ closestVertex ].nearestAtom ];
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
				if ( _molecule->getAtom( i ) == nullptr )
				{
					continue;
				}

				const std::vector<Vec3f> & trianglePoints = atomsToTriangles[ i ];

				_atomsToTriangles[ i ].first = uint( _vertices.size() );
				_atomsToTriangles[ i ].count = uint( trianglePoints.size() );

				std::vector<uint> indices = std::vector<uint>( trianglePoints.size() );
				int				  index	  = int( _vertices.size() );
				std::iota( indices.begin(), indices.end(), index );
				_indices.insert( _indices.end(), indices.begin(), indices.end() );

				_vertices.insert( _vertices.end(), trianglePoints.begin(), trianglePoints.end() );

				std::vector<uint> ids = std::vector<uint>( trianglePoints.size() );
				std::fill( ids.begin(), ids.end(), _molecule->getAtom( i )->getId() );
				_ids.insert( _ids.end(), ids.begin(), ids.end() );
			}

			_vertices.shrink_to_fit();
			_indices.shrink_to_fit();
			_ids.shrink_to_fit();

			chrono2.stop();
			VTX_INFO( "Triangles sorting done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
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

		void SolventExcludedSurface::refreshColors()
		{
			_colors.resize( _indiceCount, Color::Rgb::WHITE );

			for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
			{
				const Atom * const atom = _molecule->getAtom( atomIdx );
				if ( atom == nullptr )
				{
					continue;
				}

				const Color::Rgb & color = _molecule->getAtomColor( atomIdx );
				std::fill( _colors.begin() + _atomsToTriangles[ atomIdx ].first,
						   _colors.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						   color );
			}

			_buffer->setColors( _colors );
			_colors.clear();
			_colors.shrink_to_fit();
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			_visibilities.resize( _indiceCount, 1 );

			for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
			{
				const Atom * const atom = _molecule->getAtom( atomIdx );
				if ( atom == nullptr )
				{
					continue;
				}

				const Residue * const residue = atom->getResiduePtr();
				const Chain * const	  chain	  = residue->getChainPtr();
				const uint			  visible = _molecule->isVisible() && chain->isVisible() && residue->isVisible();

				if ( visible == 0 )
				{
					std::fill(
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first,
						_visibilities.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].count,
						0 );
				}
			}

			_buffer->setVisibilities( _visibilities );
			_visibilities.clear();
			_visibilities.shrink_to_fit();
		}

		void SolventExcludedSurface::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
		{
			_selections.resize( _indiceCount, 0 );

			if ( p_selection != nullptr )
			{
				if ( p_selection->getFullySelectedChildCount() == _molecule->getRealChainCount() )
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

			_buffer->setSelections( _selections );
			_selections.clear();
			_selections.shrink_to_fit();
		}

		void SolventExcludedSurface::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::TriangleSES>( this, VTX::ID::View::D3_TRIANGLE_SES ) );
		}
	} // namespace Model
} // namespace VTX
