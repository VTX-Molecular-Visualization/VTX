#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "math/marching_cube.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"

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
			Tool::Chrono chrono;
			chrono.start();
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

			struct AtomData
			{
				int index;
			};

			std::vector<std::vector<AtomData>> atomData
				= std::vector<std::vector<AtomData>>( _gridAtoms.getCellCount(), std::vector<AtomData>() );
			const std::vector<Vec3f> & atomPositions = _molecule->getAtomPositionFrame( 0 );
			for ( uint i = 0; i < atomPositions.size(); ++i )
			{
				const uint hash = _gridAtoms.gridHash( atomPositions[ i ] );
				atomData[ hash ].emplace_back( AtomData { int( i ) } );
			}

			// Compute SES grid and compute SDF.
			const float voxelSize	= 0.5f;
			Vec3i		sesGridSize = Vec3i( Util::Math::ceil( size / voxelSize ) );

			_gridSES = Object3D::Helper::Grid( min, Vec3f( voxelSize ), sesGridSize );

			struct SESGridData
			{
				float sdf;
			};

			// SES grid data.
			std::vector<SESGridData> sesGridData
				= std::vector<SESGridData>( _gridSES.getCellCount(), SESGridData { probeRadius } );

			// Store boundary references.
			std::vector<uint> sesGridDataBoundary = std::vector<uint>();

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
						SESGridData & gridData		  = sesGridData[ sesGridHash ];

						// Get corresponding acceleration grid cell hash.
						const Vec3f sesGridCellWorldPosition = _gridSES.worldPosition( sesGridPosition );
						const Vec3i atomGridPosition		 = _gridAtoms.gridPosition( sesGridCellWorldPosition );

						// Loop over the 27 cells to visit.
						bool found = false;
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
										const uint distance
											= ( atomPositions[ atom.index ] - sesGridCellWorldPosition ).length();

										// Inside.
										if ( distance < voxelSize )
										{
											gridData.sdf = -voxelSize;
											found		 = true;
											break;
										}
										// Boundary.
										else if ( int distanceWithProbe
												  = distance - probeRadius
														- _molecule->getAtom( atom.index )->getVdwRadius()
													< 0 )
										{
											sesGridDataBoundary.push_back( sesGridHash );
											gridData.sdf = -voxelSize;
											found		 = true;
											break;
										}
									}
								}
							}
						}
					}
				}
			}

			// SDF refinement.
			const int cellsToVisitCount = Util::Math::ceil( probeRadius / probeRadius );
			VTX_INFO( "{} / {}", sesGridDataBoundary.size(), sesGridData.size() );
			for ( const uint sesGridHash : sesGridDataBoundary )
			{
				const Vec3i	  sesGridPosition			  = _gridSES.gridPosition( sesGridHash );
				const Vec3f	  sesWorldPosition			  = _gridSES.worldPosition( sesGridPosition );
				SESGridData & gridDataToVisit			  = sesGridData[ sesGridHash ];
				float		  minDistanceWithOutsidePoint = FLOAT_MAX;
				bool		  found						  = false;
				for ( int ox = -cellsToVisitCount; ox <= cellsToVisitCount; ++ox )
				{
					for ( int oy = -cellsToVisitCount; oy <= cellsToVisitCount; ++oy )
					{
						for ( int oz = -cellsToVisitCount; oz <= cellsToVisitCount; ++oz )
						{
							const Vec3i gridPositionToVisit = sesGridPosition + Vec3i( ox, oy, oz );
							if ( gridPositionToVisit.x < 0 || gridPositionToVisit.y < 0 || gridPositionToVisit.z < 0
								 || gridPositionToVisit.x >= _gridSES.size.x || gridPositionToVisit.y >= _gridSES.size.y
								 || gridPositionToVisit.z >= _gridSES.size.z )
							{
								continue;
							}

							// VTX_INFO( "{}", glm::to_string( gridPositionToVisit ) );

							const uint	  hashToVisit		   = _gridSES.gridHash( gridPositionToVisit );
							const Vec3f	  worldPositionToVisit = _gridSES.worldPosition( gridPositionToVisit );
							SESGridData & gridDataToVisit	   = sesGridData[ hashToVisit ];

							// If outside.
							if ( gridDataToVisit.sdf == probeRadius )
							{
								float distance = ( worldPositionToVisit - sesWorldPosition ).length();
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

			// Marching cube to extract mesh.
			const Math::MarchingCube marchingCube = Math::MarchingCube();
			for ( uint x = 0; x < uint( _gridSES.size.x ) - 1; ++x )
			{
				for ( uint y = 0; y < uint( _gridSES.size.y ) - 1; ++y )
				{
					for ( uint z = 0; z < uint( _gridSES.size.z ) - 1; ++z )
					{
						Math::MarchingCube::GridCell cell
							= { { _gridSES.worldPosition( Vec3i( x, y, z ) ),
								  { _gridSES.worldPosition( Vec3i( x + 1, y, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y + 1, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y + 1, z ) ) },
								  { _gridSES.worldPosition( Vec3i( x + 1, y + 1, z + 1 ) ) },
								  { _gridSES.worldPosition( Vec3i( x, y + 1, z + 1 ) ) } },
								{ sesGridData[ _gridSES.gridHash( Vec3i( x, y, z ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y, z ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y, z + 1 ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x, y, z + 1 ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x, y + 1, z ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y + 1, z ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x + 1, y + 1, z + 1 ) ) ].sdf,
								  sesGridData[ _gridSES.gridHash( Vec3i( x, y + 1, z + 1 ) ) ].sdf } };

						std::vector<std::vector<Vec3f>> cellTriangles = marchingCube.triangulateCell( cell, 0.1f );
						for ( uint i = 0; i < uint( cellTriangles.size() ); ++i )
						{
							assert( cellTriangles[ i ].size() == 3 );
							_indices.insert( _indices.end(),
											 { uint( _vertices.size() ),
											   uint( _vertices.size() ) + 1,
											   uint( _vertices.size() ) + 2 } );
							_vertices.insert( _vertices.end(), cellTriangles[ i ].begin(), cellTriangles[ i ].end() );
						}
					}
				}
			}

			_normals	  = std::vector( _vertices.size(), Vec3f( 1.f, 1.f, 1.f ) );
			_colors		  = std::vector( _vertices.size(), Color::Rgb( 1.f, 1.f, 1.f ) );
			_visibilities = std::vector<uint>( _vertices.size(), 1 );

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}
	} // namespace Model
} // namespace VTX
