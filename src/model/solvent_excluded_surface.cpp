#include "solvent_excluded_surface.hpp"
#include "atom.hpp"
#include "molecule.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"

namespace VTX
{
	namespace Model
	{
		SolventExcludedSurface::SolventExcludedSurface( Molecule * const p_molecule ) :
			BaseModel3D( VTX::ID::Model::MODEL_SOLVENT_EXCLUDED_SURFACE ), _molecule( p_molecule )
		{
		}

		void SolventExcludedSurface::print() const { VTX_DEBUG( "ICI" ); }

		void SolventExcludedSurface::_init()
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_INFO( "Creating SES..." );

			//// Step 1: sort atoms in grid.
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

			//// Step 2: compute SES grid and classiy points.

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

			// Loop over cells.
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
						for ( uint ox = -1; ox <= 1 && !found; ++ox )
						{
							for ( uint oy = -1; oy <= 1 && !found; ++oy )
							{
								for ( uint oz = -1; oz <= 1 && !found; ++oz )
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

									// Compute distance field.
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

			// Distance field refinement
			const uint cellsToVisitCount = Util::Math::ceil( probeRadius / probeRadius );
			for ( const uint sesGridHash : sesGridDataBoundary )
			{
				const Vec3i sesGridPosition = _gridSES.gridPosition( sesGridHash );
				for ( uint ox = -cellsToVisitCount; ox <= cellsToVisitCount; ++ox )
				{
					for ( uint oy = -cellsToVisitCount; oy <= cellsToVisitCount; ++oy )
					{
						for ( uint oz = -cellsToVisitCount; oz <= cellsToVisitCount; ++oz )
						{
							const Vec3i gridPositionToVisit = sesGridPosition + Vec3i( ox, oy, oz );
							uint		hashToVisit			= _gridSES.gridHash( gridPositionToVisit );
						}
					}
				}
			}

			chrono.stop();
			VTX_INFO( "SES created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void SolventExcludedSurface::_fillBuffer() {}

		void SolventExcludedSurface::_computeAABB() const {}

		void SolventExcludedSurface::_instantiate3DViews() {}

	} // namespace Model
} // namespace VTX
