#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/helper/scene.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/system/uid.hpp"
#include <optional>
#include <renderer/renderer.hpp>

namespace VTX::App::Action::Selection
{

	void SelectAll::execute()
	{
		REG().view<System::Selection>().each(
			[]( const ECS::Entity p_ent, System::Selection & p_selection )
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
			[]( const ECS::Entity p_ent, System::Selection & p_selection )
			{
				if ( p_selection.atoms.none() )
				{
					return;
				}

				patchSelection( p_ent, Util::Math::BitSet( p_selection.atoms.size() ) );
			}
		);
	}

	void Clear::execute( const ECS::Entity p_ent, const E_MODE p_mode )
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
				[ p_ent ]( const ECS::Entity ent, System::Selection & p_selection )
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
		auto firstEntOpt  = Helper::Scene::findSystemByAtomUID( first );
		auto secondEntOpt = Helper::Scene::findSystemByAtomUID( second );

		if ( firstEntOpt )
		{
			ECS::Entity	 firstEnt  = *firstEntOpt;
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

			ECS::Entity firstEnt = *firstEntOpt;

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
} // namespace VTX::App::Action::Selection
