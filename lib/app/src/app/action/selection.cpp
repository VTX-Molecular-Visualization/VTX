#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/ecs.hpp"
#include "app/helper/scene.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/system/uid.hpp"
#include <renderer/renderer.hpp>

namespace
{
	template<VTX::Core::Struct::E_SYSTEM_ITEM ITEM>
	void _setSelected(
		const VTX::App::ECS::Entity				  p_ent,
		const VTX::Core::Struct::IndexRangeList & p_ranges,
		const bool								  p_append
	)
	{
		VTX::App::ACTION().execute<VTX::App::Action::Selection::SetSelected<ITEM>>( p_ent, p_ranges, true, p_append );
	}

	void _pickAtom(
		const VTX::App::ECS::Entity						 p_ent,
		const VTX::Core::Struct::Topology &				 p_topology,
		const VTX::Index								 p_firstAtomIndex,
		const VTX::Index * const						 p_secondAtomIndex,
		const VTX::App::Action::Selection::E_GRANULARITY p_granularity,
		const bool										 p_append
	)
	{
		VTX::Core::Struct::IndexRangeList toSelect;

		switch ( p_granularity )
		{
		case VTX::App::Action::Selection::E_GRANULARITY::ATOM:
			toSelect.addRange( p_firstAtomIndex );
			if ( p_secondAtomIndex != nullptr )
			{
				toSelect.addRange( *p_secondAtomIndex );
			}
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::ATOM>( p_ent, toSelect, p_append );
			return;

		case VTX::App::Action::Selection::E_GRANULARITY::RESIDUE:
			toSelect.addRange( p_topology.getAtomResidueIndex( p_firstAtomIndex ) );
			if ( p_secondAtomIndex != nullptr )
			{
				toSelect.addRange( p_topology.getAtomResidueIndex( *p_secondAtomIndex ) );
			}
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::RESIDUE>( p_ent, toSelect, p_append );
			return;

		case VTX::App::Action::Selection::E_GRANULARITY::CHAIN:
			toSelect.addRange( p_topology.getResidueChainIndex( p_topology.getAtomResidueIndex( p_firstAtomIndex ) ) );
			if ( p_secondAtomIndex != nullptr )
			{
				toSelect.addRange(
					p_topology.getResidueChainIndex( p_topology.getAtomResidueIndex( *p_secondAtomIndex ) )
				);
			}
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::CHAIN>( p_ent, toSelect, p_append );
			return;

		case VTX::App::Action::Selection::E_GRANULARITY::SYSTEM:
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::SYSTEM>( p_ent, toSelect, p_append );
			return;

		default: assert( false ); return;
		}
	}

	void _pickResidue(
		const VTX::App::ECS::Entity				   p_ent,
		const VTX::Core::Struct::Topology &		   p_topology,
		const VTX::Index						   p_residueIndex,
		VTX::App::Action::Selection::E_GRANULARITY p_granularity,
		const bool								   p_append
	)
	{
		VTX::Core::Struct::IndexRangeList toSelect;

		if ( p_granularity == VTX::App::Action::Selection::E_GRANULARITY::ATOM )
		{
			p_granularity = VTX::App::Action::Selection::E_GRANULARITY::RESIDUE;
		}

		switch ( p_granularity )
		{
		case VTX::App::Action::Selection::E_GRANULARITY::RESIDUE:
			toSelect.addRange( p_residueIndex );
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::RESIDUE>( p_ent, toSelect, p_append );
			return;

		case VTX::App::Action::Selection::E_GRANULARITY::CHAIN:
			toSelect.addRange( p_topology.getResidueChainIndex( p_residueIndex ) );
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::CHAIN>( p_ent, toSelect, p_append );
			return;

		case VTX::App::Action::Selection::E_GRANULARITY::SYSTEM:
			_setSelected<VTX::Core::Struct::E_SYSTEM_ITEM::SYSTEM>( p_ent, toSelect, p_append );
			return;

		default: assert( false ); return;
		}
	}
} // namespace

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
		const Vec2i		 picked = RENDERER().getPickedIds( p_mousePos.x, p_mousePos.y );
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

		// Look for atom.
		auto firstEntOpt = Helper::Scene::findSystemByAtomUID( first );
		if ( firstEntOpt )
		{
			const ECS::Entity firstEnt = *firstEntOpt;
			const auto &	  topology = REG().get<Core::Struct::Topology>( firstEnt );
			const auto &	  uid	   = REG().get<System::UID>( firstEnt );

			const Index	  firstAtomIndex  = uid.getAtomIndex( first );
			Index		  secondAtomIndex = 0;
			const Index * secondAtom	  = nullptr;

			if ( second != INVALID_UID )
			{
				auto secondEntOpt = Helper::Scene::findSystemByAtomUID( second );
				if ( secondEntOpt )
				{
					const ECS::Entity secondEnt = *secondEntOpt;
					assert( secondEnt == firstEnt );
					secondAtomIndex = uid.getAtomIndex( second );
					secondAtom		= &secondAtomIndex;
				}
			}

			_pickAtom( firstEnt, topology, firstAtomIndex, secondAtom, p_granularity, p_append );
		}
		else
		{
			// Look for residue.
			firstEntOpt = Helper::Scene::findSystemByResidueUID( first );
			assert( firstEntOpt );
			assert( second == INVALID_UID );

			const ECS::Entity firstEnt	   = *firstEntOpt;
			const auto &	  topology	   = REG().get<Core::Struct::Topology>( firstEnt );
			const auto &	  uid		   = REG().get<System::UID>( firstEnt );
			const Index		  residueIndex = uid.getResidueIndex( first );

			_pickResidue( firstEnt, topology, residueIndex, p_granularity, p_append );
		}
	}
} // namespace VTX::App::Action::Selection
