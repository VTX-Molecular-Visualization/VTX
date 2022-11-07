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
			case SolventExcludedSurface::Mode::CPU:
				_refreshCPU();
				MeshTriangle::_init();
				break;
			case SolventExcludedSurface::Mode::GPU:
			{
				_buffer->makeContextCurrent();
				_refreshGPU();
				_buffer->doneContextCurrent();
				break;
			}
			default: return;
			}
		}

		void SolventExcludedSurface::_refreshGPU()
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

			std::vector<std::vector<uint>> atomGridDataTmp
				= std::vector<std::vector<uint>>( gridAtoms.getCellCount(), std::vector<uint>() );

			const std::vector<Vec3f> & atomPositions = _molecule->getCurrentAtomPositionFrame();
			std::vector<float>		   atomVdwRadius = std::vector<float>();
			atomVdwRadius.resize( atomPositions.size() );

			// Store atom indices in acceleration grid.
			// TODO: remove this loop and create directly 1D arrays?
			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				const uint hash = gridAtoms.gridHash( atomPositions[ i ] );
				atomGridDataTmp[ hash ].emplace_back( i );
				atomVdwRadius[ i ] = _molecule->getAtom( i )->getVdwRadius();
			}

			// Linerize data in 1D arrays.
			std::vector<AtomGridDataSorted> atomGridDataSorted
				= std::vector<AtomGridDataSorted>( gridAtoms.getCellCount(), AtomGridDataSorted { 0, 0 } );
			std::vector<uint> atomIndexSorted = std::vector<uint>();

			for ( uint i = 0; i < atomGridDataTmp.size(); ++i )
			{
				const std::vector<uint> & data = atomGridDataTmp[ i ];
				if ( data.size() > 0 )
				{
					atomGridDataSorted[ i ] = AtomGridDataSorted { int( atomIndexSorted.size() ), int( data.size() ) };
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
			Vec3i cellsToVisitCount = Util::Math::ceil( atomGridCellSize / Vec3f( PROBE_RADIUS + maxVdWRadius ) );

			// SES grid data.
			std::vector<SESGridData> sesGridData
				= std::vector<SESGridData>( gridSES.getCellCount(), SESGridData { PROBE_RADIUS, -1 } );

			// Create GPU worker.
			Worker::GpuComputer worker( IO::FilePath( "ses/create_sdf.comp" ) );

			// Create SSBOs.
			// Input.
			const VTX::Renderer::GL::BufferStorage ssboAtomGridDataSorted = VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomGridDataSorted );
			const VTX::Renderer::GL::BufferStorage ssboAtomIndexSorted = VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomIndexSorted );
			const VTX::Renderer::GL::BufferStorage ssboAtomPosition = VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomPositions );
			const VTX::Renderer::GL::BufferStorage ssboAtomVdwRadius = VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, atomVdwRadius );
			// Output.
			VTX::Renderer::GL::BufferStorage ssboSesGridData = VTX::Renderer::GL::BufferStorage(
				VTX::Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER, sesGridData );

			// TODO: clear CPU buffers?

			ssboAtomGridDataSorted.bind( 0 );
			ssboAtomIndexSorted.bind( 1 );
			ssboAtomPosition.bind( 2 );
			ssboAtomVdwRadius.bind( 3 );
			ssboSesGridData.bind( 4 );

			// Set uniforms.
			worker.getProgram().use();

			worker.getProgram().setVec3f( "uGridAtomWorldOrigin", gridAtoms.worldOrigin );
			worker.getProgram().setVec3f( "uGridSESWorldOrigin", gridSES.worldOrigin );
			worker.getProgram().setVec3i( "uGridAtomSize", gridAtoms.size );
			worker.getProgram().setVec3i( "uGridSESSize", gridSES.size );
			worker.getProgram().setVec3f( "uGridAtomCellSize", gridAtoms.cellSize );
			worker.getProgram().setVec3f( "uGridSESCellSize", gridSES.cellSize );
			worker.getProgram().setVec3i( "uCellsToVisitCount", cellsToVisitCount );
			worker.getProgram().setFloat( "uProbeRadius", PROBE_RADIUS );
			worker.getProgram().setFloat( "uVoxelSize", VOXEL_SIZE );

			// std::vector<SESGridData> * const sesGridData2 = ssboSesGridData.map<std::vector<SESGridData>>(
			//	0,
			//	gridSES.getCellCount() * sizeof( SESGridData ),
			//	VTX::Renderer::GL::BufferStorage::Flags::MAP_READ_BIT );

			VTX_DEBUG( "SES grid size: {}", gridSES.getCellCount() );
			// worker.setBarrier( GL_ALL_BARRIER_BITS );
			// worker.setForce( true );
			worker.start( gridSES.size );

			ssboSesGridData.getData( 0, gridSES.getCellCount() * sizeof( SESGridData ), &sesGridData[ 0 ] );

			ssboAtomGridDataSorted.unbind();
			ssboAtomIndexSorted.unbind();
			ssboAtomPosition.unbind();
			ssboAtomVdwRadius.unbind();
			ssboSesGridData.unbind();

			chrono2.stop();
			VTX_INFO( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );

			std::ofstream outFile( "GPU_DATA.txt" );
			for ( const auto & e : sesGridData )
				outFile << std::to_string( e.sdf ) + " " + std::to_string( e.nearestAtom ) << "\n";
			// for ( const auto & e : atomGridDataSorted )
			// outFile << std::to_string( e.first ) + " " + std::to_string( e.count ) << "\n";
			outFile.close();

			// chrono2.start();

			/*
			// SDF refinement.
			cellsToVisitCount = Util::Math::ceil( atomGridCellSize / Vec3f( PROBE_RADIUS + VOXEL_SIZE ) );

			for ( const uint sesGridHash : sesGridDataBoundary )
			{
				const Vec3i	  sesGridPosition			  = gridSES.gridPosition( sesGridHash );
				const Vec3f	  sesWorldPosition			  = gridSES.worldPosition( sesGridPosition );
				SESGridData & gridDataToVisit			  = sesGridData[ sesGridHash ];
				float		  minDistanceWithOutsidePoint = FLOAT_MAX;
				bool		  found						  = false;
				for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x; ++ox )
				{
					for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y; ++oy )
					{
						for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z; ++oz )
						{
							const Vec3i gridPositionToVisit = sesGridPosition + Vec3i( ox, oy, oz );

							if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0 || gridPositionToVisit.z < 0
								 || gridPositionToVisit.x >= gridSES.size.x || gridPositionToVisit.y >= gridSES.size.y
								 || gridPositionToVisit.z >= gridSES.size.z )
							{
								continue;
							}

							const uint	  hashToVisit		   = gridSES.gridHash( gridPositionToVisit );
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
					gridDataToVisit.sdf = PROBE_RADIUS - minDistanceWithOutsidePoint;
				}
			}

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
						for ( uint triangle = 0; triangle < uint( cellTriangles.size() ); ++triangle )
						{
							assert( cellTriangles[ triangle ].size() == 3 );

							// Get closest atom.
							float closestDistance = FLOAT_MAX;
							uint  closestVertex	  = 0;
							Vec3f centroid = Vec3f( cellTriangles[ triangle ][ 0 ] + cellTriangles[ triangle ][ 1 ]
													+ cellTriangles[ triangle ][ 2 ] )
											 / 3.f;
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
							triangles.insert(
								triangles.end(), cellTriangles[ triangle ].begin(), cellTriangles[ triangle ].end() );
						}
					}
				}
			}

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Fill buffers with sorted values and store data as triangle range per atoms.
			_atomsToTriangles = std::vector<std::pair<uint, uint>>( atomPositions.size(), std::pair<uint, uint>() );
			for ( uint i = 0; i < atomsToTriangles.size(); ++i )
			{
				if ( _molecule->getAtom( i ) == nullptr )
				{
					continue;
				}

				const std::vector<Vec3f> & trianglePoints = atomsToTriangles[ i ];

				_atomsToTriangles[ i ].first  = uint( _vertices.size() );
				_atomsToTriangles[ i ].second = uint( trianglePoints.size() );

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

			refreshColors();
			refreshVisibilities();

			assert( _vertices.size() == _indices.size() );
			assert( _vertices.size() == _normals.size() );
			assert( _vertices.size() == _ids.size() );
			assert( _vertices.size() == _colors.size() );
			assert( _vertices.size() == _visibilities.size() );

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
			*/
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
			std::vector<AtomGridDataSorted> atomGridDataSorted
				= std::vector<AtomGridDataSorted>( gridAtoms.getCellCount(), AtomGridDataSorted { 0, 0 } );
			std::vector<uint> atomIndexSorted = std::vector<uint>();

			for ( uint i = 0; i < atomGridDataTmp.size(); ++i )
			{
				const std::vector<uint> & data = atomGridDataTmp[ i ];
				if ( data.size() > 0 )
				{
					atomGridDataSorted[ i ] = AtomGridDataSorted { int( atomIndexSorted.size() ), int( data.size() ) };
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

			// Store boundary references.
			std::set<uint> sesGridDataBoundary = std::set<uint>();

			// Loop over cells
			Vec3f cellsToVisitCount = Util::Math::ceil( atomGridCellSize / Vec3f( PROBE_RADIUS + maxVdWRadius ) );
			for ( uint x = 0; x < uint( gridSES.size.x ); ++x )
			{
				for ( uint y = 0; y < uint( gridSES.size.y ); ++y )
				{
					for ( uint z = 0; z < uint( gridSES.size.z ); ++z )
					{
						// Get corresponding ses grid data.
						const Vec3i	  sesGridPosition = Vec3i( x, y, z );
						const uint	  sesGridHash	  = gridSES.gridHash( sesGridPosition );
						SESGridData & gridData		  = sesGridData[ sesGridHash ];

						// Get corresponding acceleration grid cell hash.
						const Vec3f sesGridCellWorldPosition = gridSES.worldPosition( sesGridPosition );
						const Vec3i atomGridPosition		 = gridAtoms.gridPosition( sesGridCellWorldPosition );

						// Loop over the 27 cells to visit.
						float minDistance = FLOAT_MAX;
						bool  found		  = false;

						////////////////////////////////////
						uint hashToVisit					   = gridAtoms.gridHash( atomGridPosition );
						uint first							   = atomGridDataSorted[ hashToVisit ].first;
						uint count							   = atomGridDataSorted[ hashToVisit ].count;
						sesGridData[ sesGridHash ].sdf		   = int( first );
						sesGridData[ sesGridHash ].nearestAtom = int( count );
						break;
						////////////////////////////////////

						for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x && !found; ++ox )
						{
							for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y && !found; ++oy )
							{
								for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z && !found; ++oz )
								{
									Vec3i offset			  = Vec3i( ox, oy, oz );
									Vec3i gridPositionToVisit = atomGridPosition + offset;

									if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0
										 || gridPositionToVisit.z < 0 || gridPositionToVisit.x >= gridAtoms.size.x
										 || gridPositionToVisit.y >= gridAtoms.size.y
										 || gridPositionToVisit.z >= gridAtoms.size.z )
									{
										continue;
									}

									uint hashToVisit = gridAtoms.gridHash( gridPositionToVisit );
									uint first		 = atomGridDataSorted[ hashToVisit ].first;
									uint count		 = atomGridDataSorted[ hashToVisit ].count;

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
												sesGridDataBoundary.insert( sesGridHash );
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
				}
			}

			chrono2.stop();
			VTX_INFO( "SDF created " + std::to_string( chrono2.elapsedTime() ) + "s" );

			std::ofstream outFile( "CPU_DATA.txt" );
			for ( const auto & e : sesGridData )
				outFile << std::to_string( e.sdf ) + " " + std::to_string( e.nearestAtom ) << "\n";
			outFile.close();
			chrono2.start();

			// SDF refinement.
			cellsToVisitCount = Util::Math::ceil( atomGridCellSize / Vec3f( PROBE_RADIUS + VOXEL_SIZE ) );

			for ( const uint sesGridHash : sesGridDataBoundary )
			{
				const Vec3i	  sesGridPosition			  = gridSES.gridPosition( sesGridHash );
				const Vec3f	  sesWorldPosition			  = gridSES.worldPosition( sesGridPosition );
				SESGridData & gridDataToVisit			  = sesGridData[ sesGridHash ];
				float		  minDistanceWithOutsidePoint = FLOAT_MAX;
				bool		  found						  = false;
				for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x; ++ox )
				{
					for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y; ++oy )
					{
						for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z; ++oz )
						{
							const Vec3i gridPositionToVisit = sesGridPosition + Vec3i( ox, oy, oz );

							if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0 || gridPositionToVisit.z < 0
								 || gridPositionToVisit.x >= gridSES.size.x || gridPositionToVisit.y >= gridSES.size.y
								 || gridPositionToVisit.z >= gridSES.size.z )
							{
								continue;
							}

							const uint	  hashToVisit		   = gridSES.gridHash( gridPositionToVisit );
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
					gridDataToVisit.sdf = PROBE_RADIUS - minDistanceWithOutsidePoint;
				}
			}

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
						for ( uint triangle = 0; triangle < uint( cellTriangles.size() ); ++triangle )
						{
							assert( cellTriangles[ triangle ].size() == 3 );

							// Get closest atom.
							float closestDistance = FLOAT_MAX;
							uint  closestVertex	  = 0;
							Vec3f centroid = Vec3f( cellTriangles[ triangle ][ 0 ] + cellTriangles[ triangle ][ 1 ]
													+ cellTriangles[ triangle ][ 2 ] )
											 / 3.f;
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
							triangles.insert(
								triangles.end(), cellTriangles[ triangle ].begin(), cellTriangles[ triangle ].end() );
						}
					}
				}
			}

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Fill buffers with sorted values and store data as triangle range per atoms.
			_atomsToTriangles = std::vector<std::pair<uint, uint>>( atomPositions.size(), std::pair<uint, uint>() );
			for ( uint i = 0; i < atomsToTriangles.size(); ++i )
			{
				if ( _molecule->getAtom( i ) == nullptr )
				{
					continue;
				}

				const std::vector<Vec3f> & trianglePoints = atomsToTriangles[ i ];

				_atomsToTriangles[ i ].first  = uint( _vertices.size() );
				_atomsToTriangles[ i ].second = uint( trianglePoints.size() );

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

			refreshColors();
			refreshVisibilities();

			assert( _vertices.size() == _indices.size() );
			assert( _vertices.size() == _normals.size() );
			assert( _vertices.size() == _ids.size() );
			assert( _vertices.size() == _colors.size() );
			assert( _vertices.size() == _visibilities.size() );

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::refreshColors()
		{
			_colors.clear();
			_colors.resize( _vertices.size(), Color::Rgb::WHITE );

			for ( uint atomIdx = 0; atomIdx < _atomsToTriangles.size(); ++atomIdx )
			{
				const Atom * const atom = _molecule->getAtom( atomIdx );
				if ( atom == nullptr )
				{
					continue;
				}

				const Color::Rgb & color = _molecule->getAtomColor( atomIdx );
				std::fill( _colors.begin() + _atomsToTriangles[ atomIdx ].first,
						   _colors.begin() + _atomsToTriangles[ atomIdx ].first + _atomsToTriangles[ atomIdx ].second,
						   color );
			}

			_colors.shrink_to_fit();
			_buffer->setColors( _colors );
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			_visibilities.clear();
			_visibilities.resize( _vertices.size(), 1 );

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
					std::fill( _visibilities.begin() + _atomsToTriangles[ atomIdx ].first,
							   _visibilities.begin() + _atomsToTriangles[ atomIdx ].first
								   + _atomsToTriangles[ atomIdx ].second,
							   0 );
				}
			}

			_visibilities.shrink_to_fit();
			_buffer->setVisibilities( _visibilities );
		}

		void SolventExcludedSurface::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
		{
			_selections.clear();
			_selections.resize( _vertices.size(), 0 );

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
											   + _atomsToTriangles[ atomIdx ].second,
										   1 );
							}
						}
					}
				}
			}

			_selections.shrink_to_fit();
			_buffer->setSelections( _selections );
		}

		void SolventExcludedSurface::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::TriangleSES>( this, VTX::ID::View::D3_TRIANGLE_SES ) );
		}
	} // namespace Model
} // namespace VTX
