#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/buffer_storage.hpp"
#include "residue.hpp"
#include "worker/gpu_computer.hpp"

namespace VTX
{
	namespace Model
	{
		SolventExcludedSurface::SolventExcludedSurface( Molecule * const p_molecule ) :
			MeshTriangle(), _molecule( p_molecule )
		{
		}

		void SolventExcludedSurface::_init()
		{
			refresh();
		}

		void SolventExcludedSurface::refresh()
		{
			switch ( _mode )
			{
			case SolventExcludedSurface::Mode::CPU: _initCPU(); break;
			case SolventExcludedSurface::Mode::GPU: _initGPU(); break;
			default: break;
			}

			MeshTriangle::_init();
		}

		void SolventExcludedSurface::_initGPU() {}

		void SolventExcludedSurface::_initCPU()
		{
			Tool::Chrono chrono, chrono2;
			chrono.start();
			chrono2.start();
			VTX_INFO( "Creating SES..." );

			// Sort atoms in acceleration grid.
			const float probeRadius	 = 1.4f;
			const float maxVdWRadius = *std::max_element(
				Atom::SYMBOL_VDW_RADIUS, Atom::SYMBOL_VDW_RADIUS + std::size( Atom::SYMBOL_VDW_RADIUS ) );
			const Object3D::Helper::AABB & molAABB = _molecule->getAABB();

			const float cellSize = probeRadius + maxVdWRadius;
			const Vec3f min		 = molAABB.getMin() - cellSize;
			const Vec3f max		 = molAABB.getMax() + cellSize;

			const Vec3f size	 = max - min;
			Vec3i		gridSize = Vec3i( Util::Math::ceil( size / cellSize ) );

			_gridAtoms = Object3D::Helper::Grid( min, Vec3f( cellSize ), gridSize );

			std::vector<std::vector<AtomData>> atomData
				= std::vector<std::vector<AtomData>>( _gridAtoms.getCellCount(), std::vector<AtomData>() );
			const std::vector<Vec3f> & atomPositions = _molecule->getAtomPositionFrame( 0 );

			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				const uint hash = _gridAtoms.gridHash( atomPositions[ i ] );
				atomData[ hash ].emplace_back( AtomData { int( i ) } );
			}

			chrono2.stop();
			VTX_INFO( "Atoms sorted in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			// Compute SES grid and compute SDF.
			const float voxelSize	= 0.5f;
			Vec3i		sesGridSize = Vec3i( Util::Math::ceil( size / voxelSize ) );

			_gridSES = Object3D::Helper::Grid( min, Vec3f( voxelSize ), sesGridSize );

			// SES grid data.
			_sesGridData = std::vector<SESGridData>( _gridSES.getCellCount(), SESGridData { probeRadius, -1 } );

			// Store boundary references.
			std::set<uint> sesGridDataBoundary = std::set<uint>();

			// Loop over cells
			for ( uint x = 0; x < uint( _gridSES.size.x ); ++x )
			{
				for ( uint y = 0; y < uint( _gridSES.size.y ); ++y )
				{
					for ( uint z = 0; z < uint( _gridSES.size.z ); ++z )
					{
						// Get corresponding ses grid data.
						const Vec3i	  sesGridPosition = Vec3i( x, y, z );
						const uint	  sesGridHash	  = _gridSES.gridHash( sesGridPosition );
						SESGridData & gridData		  = _sesGridData[ sesGridHash ];

						// Get corresponding acceleration grid cell hash.
						const Vec3f sesGridCellWorldPosition = _gridSES.worldPosition( sesGridPosition );
						const Vec3i atomGridPosition		 = _gridAtoms.gridPosition( sesGridCellWorldPosition );

						// Loop over the 27 cells to visit.
						float minDistance = FLOAT_MAX;
						bool  found		  = false;
						for ( int ox = -1; ox <= 1 && !found; ++ox )
						{
							for ( int oy = -1; oy <= 1 && !found; ++oy )
							{
								for ( int oz = -1; oz <= 1 && !found; ++oz )
								{
									Vec3i offset			  = Vec3i( ox, oy, oz );
									Vec3i gridPositionToVisit = atomGridPosition + offset;

									if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0
										 || gridPositionToVisit.z < 0 || gridPositionToVisit.x >= _gridAtoms.size.x
										 || gridPositionToVisit.y >= _gridAtoms.size.y
										 || gridPositionToVisit.z >= _gridAtoms.size.z )
									{
										continue;
									}

									uint hashToVisit = _gridAtoms.gridHash( gridPositionToVisit );

									// Compute SDF.
									for ( const AtomData & atom : atomData[ hashToVisit ] )
									{
										float distance = Util::Math::distance( atomPositions[ atom.index ],
																			   sesGridCellWorldPosition );

										// Inside.
										if ( distance < voxelSize )
										{
											gridData.sdf		 = -voxelSize;
											gridData.nearestAtom = -1;
											found				 = true;
											// Don't need to loop over other cells.
											break;
										}
										// Boundary.
										else
										{
											distance
												-= ( probeRadius + _molecule->getAtom( atom.index )->getVdwRadius() );
											if ( distance < 0.f )
											{
												sesGridDataBoundary.insert( sesGridHash );
												gridData.sdf = -voxelSize;
												if ( distance < minDistance )
												{
													minDistance			 = distance;
													gridData.nearestAtom = atom.index;
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
			chrono2.start();

			// SDF refinement.
			const Vec3i cellsToVisitCount = Util::Math::ceil( cellSize / Vec3f( probeRadius + voxelSize ) );

			// VTX_DEBUG( "{}", glm::to_string( cellsToVisitCount ) );
			// VTX_INFO( "{} / {}", sesGridDataBoundary.size(), sesGridData.size() );
			for ( const uint sesGridHash : sesGridDataBoundary )
			{
				// VTX_DEBUG( "{}", sesGridHash );
				const Vec3i	  sesGridPosition			  = _gridSES.gridPosition( sesGridHash );
				const Vec3f	  sesWorldPosition			  = _gridSES.worldPosition( sesGridPosition );
				SESGridData & gridDataToVisit			  = _sesGridData[ sesGridHash ];
				float		  minDistanceWithOutsidePoint = FLOAT_MAX;
				bool		  found						  = false;
				for ( int ox = -cellsToVisitCount.x; ox <= cellsToVisitCount.x; ++ox )
				{
					for ( int oy = -cellsToVisitCount.y; oy <= cellsToVisitCount.y; ++oy )
					{
						for ( int oz = -cellsToVisitCount.z; oz <= cellsToVisitCount.z; ++oz )
						{
							const Vec3i gridPositionToVisit = sesGridPosition + Vec3i( ox, oy, oz );
							// VTX_INFO( "{}", glm::to_string( gridPositionToVisit ) );

							if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0 || gridPositionToVisit.z < 0
								 || gridPositionToVisit.x >= _gridSES.size.x || gridPositionToVisit.y >= _gridSES.size.y
								 || gridPositionToVisit.z >= _gridSES.size.z )
							{
								continue;
							}

							const uint	  hashToVisit		   = _gridSES.gridHash( gridPositionToVisit );
							const Vec3f	  worldPositionToVisit = _gridSES.worldPosition( gridPositionToVisit );
							SESGridData & gridDataToVisit	   = _sesGridData[ hashToVisit ];

							// If outside.
							if ( gridDataToVisit.sdf == probeRadius )
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
					gridDataToVisit.sdf = probeRadius - minDistanceWithOutsidePoint;
				}
			}

			chrono2.stop();
			VTX_INFO( "SDF boundary created " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			_atomToTriangles = std::vector<std::vector<uint>>( atomPositions.size(), std::vector<uint>() );

			// Marching cube to extract mesh.
			const Math::MarchingCube marchingCube = Math::MarchingCube();
			for ( uint x = 0; x < uint( _gridSES.size.x ) - 1; ++x )
			{
				for ( uint y = 0; y < uint( _gridSES.size.y ) - 1; ++y )
				{
					for ( uint z = 0; z < uint( _gridSES.size.z ) - 1; ++z )
					{
						SESGridData gridData[ 8 ] = { _sesGridData[ _gridSES.gridHash( Vec3i( x, y, z ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y, z ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y, z + 1 ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x, y, z + 1 ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x, y + 1, z ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y + 1, z ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y + 1, z + 1 ) ) ],
													  _sesGridData[ _gridSES.gridHash( Vec3i( x, y + 1, z + 1 ) ) ] };

						Math::MarchingCube::GridCell cell
							= { { _gridSES.worldPosition( Vec3i( x, y, z ) ),
								  { _gridSES.worldPosition( Vec3i( x + 1, y, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y + 1, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y + 1, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y + 1, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y + 1, z + 1 ) ) } },
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
							_indices.insert( _indices.end(),
											 { uint( _vertices.size() ),
											   uint( _vertices.size() ) + 1,
											   uint( _vertices.size() ) + 2 } );
							_vertices.insert(
								_vertices.end(), cellTriangles[ triangle ].begin(), cellTriangles[ triangle ].end() );

							// Get closest point to store corresponding residue.
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

							_atomToTriangles[ gridData[ closestVertex ].nearestAtom ].push_back(
								uint( _vertices.size() ) - 3 );
						}
						// TODO: find a way to map triangles with closest ses grid point.
					}
				}
			}

			chrono2.stop();
			VTX_INFO( "Marching cube done in " + std::to_string( chrono2.elapsedTime() ) + "s" );
			chrono2.start();

			recomputeNormals();
			chrono2.stop();
			VTX_INFO( "Normals computed in " + std::to_string( chrono2.elapsedTime() ) + "s" );

			refreshColors();
			refreshVisibilities();

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::refreshColors()
		{
			_colors = std::vector<Color::Rgb>( _vertices.size() );

			for ( uint atomIdx = 0; atomIdx < _atomToTriangles.size(); ++atomIdx )
			{
				for ( const uint triangleFirstVertice : _atomToTriangles[ atomIdx ] )
				{
					_colors[ triangleFirstVertice + 0 ] = _molecule->getAtomColor( atomIdx );
					_colors[ triangleFirstVertice + 1 ] = _molecule->getAtomColor( atomIdx );
					_colors[ triangleFirstVertice + 2 ] = _molecule->getAtomColor( atomIdx );
				}
			}

			_colors.shrink_to_fit();
			_buffer->setColors( _colors );
		}

		void SolventExcludedSurface::refreshVisibilities()
		{
			_visibilities = std::vector<uint>( _vertices.size(), 1 );
			for ( uint atomIdx = 0; atomIdx < _atomToTriangles.size(); ++atomIdx )
			{
				const Atom * const	  atom	  = _molecule->getAtom( atomIdx );
				const Residue * const residue = atom->getResiduePtr();
				const Chain * const	  chain	  = residue->getChainPtr();
				const uint			  visible = _molecule->isVisible() && chain->isVisible() && residue->isVisible();

				if ( visible == 0 )
				{
					for ( const uint triangleFirstVertice : _atomToTriangles[ atomIdx ] )
					{
						_visibilities[ triangleFirstVertice + 0 ] = 0;
						_visibilities[ triangleFirstVertice + 1 ] = 0;
						_visibilities[ triangleFirstVertice + 2 ] = 0;
					}
				}
			}

			_visibilities.shrink_to_fit();
			_buffer->setVisibilities( _visibilities );
		}
	} // namespace Model
} // namespace VTX
