#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/helper/scene.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/system/gridAtomList.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "util/math/range_list.hpp"
#include <optional>
#include <renderer/renderer.hpp>

namespace VTX::App::Action::Selection
{
	void SetSelectedItem::execute(
		const Entity						 p_ent,
		const Core::Struct::E_SYSTEM_ITEM	 p_item,
		const Core::Struct::IndexRangeList & p_ranges,
		const bool							 p_selected,
		const bool							 p_append
	)
	{
		switch ( p_item )
		{
		case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
			SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_ranges, p_selected, p_append );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CATEGORY:
			SetSelected<Core::Struct::E_SYSTEM_ITEM::CATEGORY>().execute( p_ent, p_ranges, p_selected, p_append );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CHAIN:
			SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_ranges, p_selected, p_append );
			break;
		case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
			SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_ranges, p_selected, p_append );
			break;
		case Core::Struct::E_SYSTEM_ITEM::ATOM:
			SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_ranges, p_selected, p_append );
			break;
		default: assert( false && "Unhandled E_SYSTEM_ITEM type in SetSelectedItem action." ); break;
		}
	}

	void SelectAll::execute()
	{
		REG().view<System::Selection>().each(
			[]( const Entity p_ent, System::Selection & p_selection )
			{
				if ( p_selection.atoms.all() )
				{
					return;
				}

				patchSelection( p_ent, Util::Math::BitSet( p_selection.atoms.size(), true ) );
			}
		);
	}

	void Clear::execute()
	{
		REG().view<System::Selection>().each(
			[]( const Entity p_ent, System::Selection & p_selection )
			{
				if ( p_selection.atoms.none() )
				{
					return;
				}

				patchSelection( p_ent, Util::Math::BitSet( p_selection.atoms.size() ) );
			}
		);
	}

	void Clear::execute( const Entity p_ent, const E_MODE p_mode )
	{
		if ( p_mode == E_MODE::THIS )
		{
			const auto & selection = REG().get<System::Selection>( p_ent );
			if ( selection.atoms.none() )
			{
				return;
			}

			patchSelection( p_ent, Util::Math::BitSet( selection.atoms.size() ) );
		}
		else
		{
			REG().view<System::Selection>().each(
				[ p_ent ]( const Entity ent, System::Selection & p_selection )
				{
					if ( p_ent != ent )
					{
						if ( p_selection.atoms.none() )
						{
							return;
						}

						patchSelection( ent, Util::Math::BitSet( p_selection.atoms.size() ) );
					}
				}
			);
		}
	}

	void Pick::execute( const Vec2i & p_mousePos, const E_GRANULARITY p_granularity, const bool p_append )
	{
		// Get renderer picking info at mouse position.
		Vec2i picked = RENDERER().getPickedIds( p_mousePos.x, p_mousePos.y );

		const UID32 first  = picked.x;
		const UID32 second = picked.y;

		// None.
		if ( first == INVALID_UID )
		{
			// Sanity check.
			assert( second == INVALID_UID );

			if ( not p_append )
			{
				ACTION().execute<Clear>();
			}

			return;
		}

		Core::Struct::IndexRangeList toSelect;
		E_GRANULARITY				 granularity = p_granularity;

		// Look for atom.
		auto firstEntOpt  = Helper::Scene::findSystemByAtomUID( first );
		auto secondEntOpt = Helper::Scene::findSystemByAtomUID( second );

		if ( firstEntOpt )
		{
			Entity		 firstEnt  = *firstEntOpt;
			const auto & topology  = REG().get<Core::Struct::Topology>( firstEnt );
			const auto & uid	   = REG().get<System::UID>( firstEnt );
			const auto & selection = REG().get<System::Selection>( firstEnt );

			const Index			 firstAtomIndex = uid.getAtomIndex( first );
			std::optional<Index> secondAtomIndex;

			bool select = not( selection.atoms.test( firstAtomIndex ) && p_append );
			if ( secondEntOpt )
			{
				assert( *secondEntOpt == firstEnt );
				secondAtomIndex = uid.getAtomIndex( second );
				select |= not( selection.atoms.test( *secondAtomIndex ) && p_append );
			}

			switch ( granularity )
			{
			case E_GRANULARITY::ATOM:
			{
				toSelect.addRange( firstAtomIndex );
				if ( secondAtomIndex )
				{
					toSelect.addRange( *secondAtomIndex );
				}

				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>(
					firstEnt, toSelect, select, p_append
				);
				break;
			}
			case E_GRANULARITY::RESIDUE:
			{
				const Index resIndex = topology.getAtomResidueIndex( firstAtomIndex );
				toSelect.addRange( resIndex );
				if ( secondAtomIndex )
				{
					const Index resIndexSecond = topology.getAtomResidueIndex( *secondAtomIndex );
					toSelect.addRange( resIndexSecond );
				}

				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>>(
					firstEnt, toSelect, select, p_append
				);
				break;
			}
			case E_GRANULARITY::CHAIN:
			{
				const Index resIndex   = topology.getAtomResidueIndex( firstAtomIndex );
				const Index chainIndex = topology.getResidueChainIndex( resIndex );
				toSelect.addRange( chainIndex );
				if ( secondAtomIndex )
				{
					const Index resIndexSecond	 = topology.getAtomResidueIndex( *secondAtomIndex );
					const Index chainIndexSecond = topology.getResidueChainIndex( resIndexSecond );
					toSelect.addRange( chainIndexSecond );
				}

				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>>(
					firstEnt, toSelect, select, p_append
				);
				break;
			}
			case E_GRANULARITY::SYSTEM:
			{
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
					firstEnt, toSelect, select, p_append
				);
				return;
			}

			default: assert( false ); break;
			}
		}
		else
		{
			// Look for residue.
			firstEntOpt = Helper::Scene::findSystemByResidueUID( first );
			assert( firstEntOpt );
			assert( second == INVALID_UID );
			if ( granularity == E_GRANULARITY::ATOM )
			{
				granularity = E_GRANULARITY::RESIDUE;
			}

			Entity firstEnt = *firstEntOpt;

			const auto & system		  = REG().get<Core::Struct::Topology>( firstEnt );
			const auto & uid		  = REG().get<System::UID>( firstEnt );
			const Index	 residueIndex = uid.getResidueIndex( first );

			bool select = not(
				Helper::System::getSelectionState( { firstEnt, Core::Struct::E_SYSTEM_ITEM::RESIDUE, residueIndex } )
					== App::System::E_SELECTION_STATE::FULL
				&& p_append
			);

			switch ( granularity )
			{
			case E_GRANULARITY::RESIDUE:
			{
				toSelect.addRange( residueIndex );
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>>(
					firstEnt, toSelect, select, p_append
				);
				break;
			}
			case E_GRANULARITY::CHAIN:
			{
				const Index chainIndex = system.getResidueChainIndex( residueIndex );
				toSelect.addRange( chainIndex );
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>>(
					firstEnt, toSelect, select, p_append
				);
				break;
			}
			case E_GRANULARITY::SYSTEM:
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
					firstEnt, toSelect, select, p_append
				);
				return;

			default: assert( false ); break;
			}
		}
	}

	VTX::App::System::GridCoord getCell( const Vec3f & p, float cellSize )
	{
		return { int( std::floor( p.x / cellSize ) ),
				 int( std::floor( p.y / cellSize ) ),
				 int( std::floor( p.z / cellSize ) ) };
	}

	struct SystemData
	{
		ECS::Entity					 entity;
		std::span<const Vec3f>		 positions;
		App::System::Selection *	 selection;
		App::System::GridAtomList *	 gridList;
		Core::Struct::IndexRangeList toSelect;
	};

	void ExtendSelectionNonSelecFirst::execute( const float threshold )
	{
		// storing systems info
		std::vector<SystemData> systems;
		for ( const ECS::Entity system : REG().view<Core::Struct::Topology>() )
		{
			systems.emplace_back(
				system,
				VTX::App::System::getCurrentAtomPositions( system ),
				&REG().get<App::System::Selection>( system ),
				&REG().get<App::System::GridAtomList>( system ),
				Core::Struct::IndexRangeList {}
			);
			auto & gridList = REG().get<App::System::GridAtomList>( system );
			for ( auto & [ coord, voxel ] : gridList.grid )
			{
				voxel.selectedCount = 0;
				voxel.viewed		= false;
			}
		}

		const float threshold2 = threshold * threshold;

		// system loop
		for ( const SystemData & testedSystem : systems )
		{
			Core::Struct::IndexRangeList toSelect;

			// system's atoms loop
			for ( size_t i = 0; i < testedSystem.selection->atoms.size(); ++i )
			{
				bool status = testedSystem.selection->atoms.test( i );
				if ( !status )
				{
					// coord atom i
					const Vec3f & p1 = testedSystem.positions[ i ];
					// voxel atom i
					VTX::App::System::GridCoord cell = getCell( testedSystem.positions[ i ], 4.0f );
					// distance of neighbor voxels to check
					float vd = int( std::floor( threshold / 4.0f ) );
					// system loop
					for ( const SystemData & testingSystem : systems )
					{
						// voxels
						auto & gridAtomList = REG().get<VTX::App::System::GridAtomList>( testingSystem.entity );
						bool   found		= false;
						// neighbor voxels loop
						for ( int dx = -vd; dx <= vd && !found; ++dx )
							for ( int dy = -vd; dy <= vd && !found; ++dy )
								for ( int dz = -vd; dz <= vd && !found; ++dz )
								{
									// neighbor voxel coord
									VTX::App::System::GridCoord neighbor { cell.x + dx, cell.y + dy, cell.z + dz };
									// neighbor voxel
									auto it = gridAtomList.grid.find( neighbor );
									// skip loop if voxel is empty or contains no selected atoms
									if ( it == gridAtomList.grid.end()
										 || ( it->second.viewed && it->second.selectedCount == 0 ) )
										continue;
									// neighbor voxel's atoms loop
									for ( const size_t j : it->second.atoms )
									{
										bool status = testingSystem.selection->atoms.test( j );
										if ( status )
										{
											// coord atom j
											const Vec3f & p2 = testingSystem.positions[ j ];

											// checking distance on axes before calculating distance i - j
											const float dx = p1.x - p2.x;
											if ( dx > threshold || dx < -threshold )
												continue;

											const float dy = p1.y - p2.y;
											if ( dy > threshold || dy < -threshold )
												continue;

											const float dz = p1.z - p2.z;
											if ( dz > threshold || dz < -threshold )
												continue;

											const float dist2 = dx * dx + dy * dy + dz * dz;

											if ( dist2 <= threshold2 )
											{
												toSelect.addRange( i );
												found = true;
												break;
											}
											// counts selected atoms in the voxel to keep checking it
											if ( !it->second.viewed )
											{
												it->second.selectedCount++;
											}
										}
									}
									// mark voxel as viewed
									it->second.viewed = true;
								}
					}
				}
			}
			ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>(
				testedSystem.entity, toSelect, true, true
			);
		}
	}

	void ExtendSelectionRes::execute()
	{
		// systems loop
		for ( const ECS::Entity system : REG().view<Core::Struct::Topology>() )
		{
			auto &						 topology		  = REG().get<Core::Struct::Topology>( system );
			auto &						 currentSelection = REG().get<App::System::Selection>( system );
			Core::Struct::IndexRangeList toSelect;

			// selected atoms loop
			for ( size_t i : currentSelection.atoms )
			{
				const Index					   resIndex = topology.getAtomResidueIndex( i );
				const Core::Struct::IndexRange range	= topology.getResidueAtomRange( resIndex );
				toSelect.addRange( range );
			}
			ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>( system, toSelect, true, true );
		}
	}

	void RevertSelection::execute()
	{
		// system loop
		for ( const ECS::Entity system : REG().view<Core::Struct::Topology>() )
		{
			auto & currentSelection = REG().get<App::System::Selection>( system );
			REG().patch<System::Selection>(
				system,
				[ &currentSelection ]( System::Selection & p_selection )
				{
					p_selection.atoms = currentSelection.atoms;
					p_selection.atoms.flipInPlace();
				}
			);
		}
	}

	void Mapping::execute( const ECS::Entity system )
	{
		int					   count { 0 };
		auto &				   gridAtomList = REG().get<VTX::App::System::GridAtomList>( system );
		std::span<const Vec3f> positions	= VTX::App::System::getCurrentAtomPositions( system );
		// atoms loop
		for ( size_t i = 0; i < positions.size(); ++i )
		{
			VTX::App::System::GridCoord cell = getCell( positions[ i ], 4.0f );
			gridAtomList.grid[ cell ].atoms.push_back( i );
		}
	}

	void ExtendSelectionSelecFirst::execute( const float threshold )
	{
		// storing systems info
		std::vector<SystemData> systems;
		for ( const ECS::Entity system : REG().view<Core::Struct::Topology>() )
		{
			systems.emplace_back(
				system,
				VTX::App::System::getCurrentAtomPositions( system ),
				&REG().get<App::System::Selection>( system ),
				&REG().get<App::System::GridAtomList>( system ),
				Core::Struct::IndexRangeList {}
			);
			auto & gridList = REG().get<App::System::GridAtomList>( system );
			for ( auto & [ coord, voxel ] : gridList.grid )
			{
				voxel.selectedCount = 0;
				voxel.viewed		= false;
			}
		}

		const float threshold2 = threshold * threshold;
		const int	vd		   = int( std::floor( threshold / 4.0f ) );

		// systems loop
		for ( const SystemData & testedSystem : systems )
		{
			// selected atoms loop
			for ( size_t i : testedSystem.selection->atoms )
			{
				// coord atom i
				const Vec3f & p1 = testedSystem.positions[ i ];
				// voxel atom i
				VTX::App::System::GridCoord cell = getCell( testedSystem.positions[ i ], 4.0f );
				// systems loop
				for ( SystemData & testingSystem : systems )
				{
					// neighbor voxels loop
					for ( int dx = -1 * vd; dx <= 1 * vd; ++dx )
						for ( int dy = -1 * vd; dy <= 1 * vd; ++dy )
							for ( int dz = -1 * vd; dz <= 1 * vd; ++dz )
							{
								// neighbor voxel coord
								VTX::App::System::GridCoord neighbor { cell.x + dx, cell.y + dy, cell.z + dz };
								// neighbor voxel
								auto it = testingSystem.gridList->grid.find( neighbor );
								// skip loop if voxel empty or contains only selected atoms
								if ( it == testingSystem.gridList->grid.end()
									 or ( it->second.viewed && it->second.atoms.size() == it->second.selectedCount ) )
									continue;

								// neighbor voxel's atoms loop
								for ( const size_t j : it->second.atoms )
								{
									bool status = testingSystem.selection->atoms.test( j );
									if ( !status )
									{
										// coord atom j
										const Vec3f & p2 = testingSystem.positions[ j ];

										// checking distance on axes before calculating distance i - j
										const float dx = p1.x - p2.x;
										if ( dx > threshold || dx < -threshold )
											continue;

										const float dy = p1.y - p2.y;
										if ( dy > threshold || dy < -threshold )
											continue;

										const float dz = p1.z - p2.z;
										if ( dz > threshold || dz < -threshold )
											continue;

										const float dist2 = dx * dx + dy * dy + dz * dz;

										if ( dist2 <= threshold2 )
										{
											if ( !testingSystem.toSelect.contains( j ) )
											{
												testingSystem.toSelect.addRange( j );
												it->second.selectedCount++;
											}
										}
									}
									// counts selected atoms in the voxel to not check it again
									else if ( status and !it->second.viewed )
									{
										it->second.selectedCount++;
									}
								}
								// mark voxel as viewed
								it->second.viewed = true;
							}
				}
			}
		}
		for ( const SystemData & v : systems )
		{
			ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>( v.entity, v.toSelect, true, true );
		}
	}
} // namespace VTX::App::Action::Selection
