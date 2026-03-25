#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/helper/scene.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/system/uid.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::Action::Selection
{

	void Clear::execute()
	{
		REG().view<System::Selection>().each(
			[]( const ECS::Entity p_ent, System::Selection & )
			{
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
					p_ent, Core::Struct::IndexRangeList(), false
				);
			}
		);
	}

	void Clear::execute( const ECS::Entity p_ent, const E_MODE p_mode )
	{
		if ( p_mode == E_MODE::THIS )
		{
			ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
				p_ent, Core::Struct::IndexRangeList(), false
			);
		}
		else
		{
			REG().view<System::Selection>().each(
				[ p_ent ]( const ECS::Entity ent, System::Selection & )
				{
					if ( p_ent != ent )
					{
						ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
							ent, Core::Struct::IndexRangeList(), false
						);
					}
				}
			);
		}
	}

	void Pick::execute( const Vec2i & p_mousePos, const E_GRANULARITY p_granularity, const bool p_append )
	{
		// Get renderer picking info at mouse position.
		Vec2i picked = RENDERER().getPickedIds( p_mousePos.x, p_mousePos.y );

		const PickingUID first	= picked.x;
		const PickingUID second = picked.y;

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
		auto firstEntOpt = Helper::Scene::findSystemByAtomUID( first );
		if ( firstEntOpt )
		{
			ECS::Entity	 firstEnt = *firstEntOpt;
			const auto & topology = REG().get<Core::Struct::Topology>( firstEnt );
			const auto & uid	  = REG().get<System::UID>( firstEnt );

			const Index firstAtomIndex = uid.getAtomIndex( first );

			auto secondEntOpt = Helper::Scene::findSystemByAtomUID( second );

			if ( not secondEntOpt )
			{
				switch ( granularity )
				{
				case E_GRANULARITY::ATOM:
					toSelect.addRange( firstAtomIndex );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				case E_GRANULARITY::RESIDUE:
				{
					const Index resIndex = topology.getAtomResidueIndex( firstAtomIndex );
					toSelect.addRange( resIndex );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				}
				case E_GRANULARITY::CHAIN:
				{
					const Index resIndex   = topology.getAtomResidueIndex( firstAtomIndex );
					const Index chainIndex = topology.getResidueChainIndex( resIndex );
					toSelect.addRange( chainIndex );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				}
				case E_GRANULARITY::SYSTEM:
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
						firstEnt, toSelect, true, p_append
					);
					return;

				default: assert( false ); break;
				}
			}
			else
			{
				ECS::Entity secondEnt = *secondEntOpt;
				assert( secondEnt == firstEnt );
				const Index secondAtomIndex = uid.getAtomIndex( second );

				switch ( granularity )
				{
				case E_GRANULARITY::ATOM:
					toSelect.addRange( firstAtomIndex );
					toSelect.addRange( secondAtomIndex );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::ATOM>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				case E_GRANULARITY::RESIDUE:
				{
					toSelect.addRange( topology.getAtomResidueIndex( firstAtomIndex ) );
					toSelect.addRange( topology.getAtomResidueIndex( secondAtomIndex ) );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				}
				case E_GRANULARITY::CHAIN:
				{
					const Index firstResIndex  = topology.getAtomResidueIndex( firstAtomIndex );
					const Index secondResIndex = topology.getAtomResidueIndex( secondAtomIndex );
					toSelect.addRange( topology.getResidueChainIndex( firstResIndex ) );
					toSelect.addRange( topology.getResidueChainIndex( secondResIndex ) );
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>>(
						firstEnt, toSelect, true, p_append
					);
					break;
				}
				case E_GRANULARITY::SYSTEM:
					ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
						firstEnt, toSelect, true, p_append
					);
					return;

				default: assert( false ); break;
				}
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

			ECS::Entity firstEnt = *firstEntOpt;

			const auto & system		  = REG().get<Core::Struct::Topology>( firstEnt );
			const auto & uid		  = REG().get<System::UID>( firstEnt );
			const Index	 residueIndex = uid.getResidueIndex( first );

			switch ( granularity )
			{
			case E_GRANULARITY::RESIDUE:
			{
				toSelect.addRange( residueIndex );
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::RESIDUE>>(
					firstEnt, toSelect, true, p_append
				);
				break;
			}
			case E_GRANULARITY::CHAIN:
			{
				const Index chainIndex = system.getResidueChainIndex( residueIndex );
				toSelect.addRange( chainIndex );
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::CHAIN>>( firstEnt, toSelect, true, p_append );
				break;
			}
			case E_GRANULARITY::SYSTEM:
				ACTION().execute<SetSelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>>(
					firstEnt, toSelect, true, p_append
				);
				return;

			default: assert( false ); break;
			}
		}
	}
} // namespace VTX::App::Action::Selection
