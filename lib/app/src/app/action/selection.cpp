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
			{ ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( p_ent, Core::Struct::IndexRangeList(), false ); }
		);
	}

	void Clear::execute( const ECS::Entity p_ent )
	{
		ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( p_ent, Core::Struct::IndexRangeList(), false );
	}

	void Pick::execute( const Vec2i & p_mousePos, const E_GRANULARITY p_granularity, const bool p_append )
	{
		// Get renderer picking info at mouse position.
		Vec2i picked = RENDERER().getPickedIds( p_mousePos.x, p_mousePos.y );

		const PickingUID first	= picked.x;
		const PickingUID second = picked.y;

		// Clear selection if not appending.
		if ( not p_append )
		{
			ACTION().execute<Clear>();
		}

		// None.
		if ( first == INVALID_UID )
		{
			// Sanity check.
			assert( second == INVALID_UID );
			return;
		}

		Core::Struct::IndexRangeList toSelect;
		E_GRANULARITY				 granularity = p_granularity;

		// Look for atom.
		auto firstEntOpt = Helper::Scene::findSystemByAtomUID( first );
		if ( firstEntOpt )
		{
			ECS::Entity	 firstEnt = *firstEntOpt;
			const auto & system	  = REG().get<Core::Struct::System>( firstEnt );
			const auto & uid	  = REG().get<System::UID>( firstEnt );

			const Index firstAtomIndex = uid.getAtomIndex( first );

			auto secondEntOpt = Helper::Scene::findSystemByAtomUID( second );

			if ( not secondEntOpt )
			{
				switch ( granularity )
				{
				case E_GRANULARITY::ATOM:
					toSelect.addRange( firstAtomIndex );
					ACTION().execute<SetSelected<Scene::E_ITEM::ATOM>>( firstEnt, toSelect );
					break;
				case E_GRANULARITY::RESIDUE:
				{
					const Index resIndex = system.getAtomResidueIndex( firstAtomIndex );
					toSelect.addRange( resIndex );
					ACTION().execute<SetSelected<Scene::E_ITEM::RESIDUE>>( firstEnt, toSelect );
					break;
				}
				case E_GRANULARITY::CHAIN:
				{
					const Index resIndex   = system.getAtomResidueIndex( firstAtomIndex );
					const Index chainIndex = system.getResidueChainIndex( resIndex );
					toSelect.addRange( chainIndex );
					ACTION().execute<SetSelected<Scene::E_ITEM::CHAIN>>( firstEnt, toSelect );
					break;
				}
				case E_GRANULARITY::SYSTEM: ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( firstEnt ); return;

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
					ACTION().execute<SetSelected<Scene::E_ITEM::ATOM>>( firstEnt, toSelect );
					break;
				case E_GRANULARITY::RESIDUE:
				{
					toSelect.addRange( system.getAtomResidueIndex( firstAtomIndex ) );
					toSelect.addRange( system.getAtomResidueIndex( secondAtomIndex ) );
					ACTION().execute<SetSelected<Scene::E_ITEM::RESIDUE>>( firstEnt, toSelect );
					break;
				}
				case E_GRANULARITY::CHAIN:
				{
					const Index firstResIndex  = system.getAtomResidueIndex( firstAtomIndex );
					const Index secondResIndex = system.getAtomResidueIndex( secondAtomIndex );
					toSelect.addRange( system.getResidueChainIndex( firstResIndex ) );
					toSelect.addRange( system.getResidueChainIndex( secondResIndex ) );
					ACTION().execute<SetSelected<Scene::E_ITEM::CHAIN>>( firstEnt, toSelect );
					break;
				}
				case E_GRANULARITY::SYSTEM: ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( firstEnt ); return;

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

			const auto & system		  = REG().get<Core::Struct::System>( firstEnt );
			const auto & uid		  = REG().get<System::UID>( firstEnt );
			const Index	 residueIndex = uid.getAtomIndex( first );

			switch ( granularity )
			{
			case E_GRANULARITY::RESIDUE:
			{
				toSelect.addRange( residueIndex );
				ACTION().execute<SetSelected<Scene::E_ITEM::RESIDUE>>( firstEnt, toSelect );
				break;
			}
			case E_GRANULARITY::CHAIN:
			{
				const Index chainIndex = system.getResidueChainIndex( residueIndex );
				toSelect.addRange( chainIndex );
				ACTION().execute<SetSelected<Scene::E_ITEM::CHAIN>>( firstEnt, toSelect );
				break;
			}
			case E_GRANULARITY::SYSTEM: ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( firstEnt ); return;

			default: assert( false ); break;
			}
		}
	}
} // namespace VTX::App::Action::Selection
