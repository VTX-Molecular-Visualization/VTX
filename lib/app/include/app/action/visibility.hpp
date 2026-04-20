#ifndef __VTX_APP_ACTION_VISIBILITY__
#define __VTX_APP_ACTION_VISIBILITY__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/topology.hpp>
#include <util/chrono.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Visibility
{
	/**
	 * @brief Set item visibility.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct SetVisible
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::IndexRangeList & p_ranges  = {},
			const bool							 p_visible = true
		)
		{
			Util::ScopedChrono timer( "App::Action::SetVisible" );

			auto &						 reg   = REG();
			Core::Struct::IndexRangeList atoms = Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );

			reg.patch<System::Visibility>(
				p_ent,
				[ &atoms, p_visible ]( System::Visibility & p_visibility )
				{
					if ( p_visible )
					{
						p_visibility.atoms.mergeInPlace( atoms );
					}
					else
					{
						p_visibility.atoms.subtractInPlace( atoms );
					}
				}
			);
		}

		void execute( const ECS::Entity p_ent, const Core::Struct::IndexRange & p_range, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_visible ); }

		void execute( const ECS::Entity p_ent, const std::vector<Index> & p_values, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_visible ); }

		void execute( const ECS::Entity p_ent, const Index p_value, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_visible ); }
	};

	struct SetVisibleSelected
	{
		void execute( const bool p_visible = true )
		{
			auto & reg = REG();

			REG().view<System::Selection>().each(
				[ &reg, p_visible ]( auto p_e, auto & p_selection )
				{
					reg.patch<System::Visibility>(
						p_e,
						[ &p_selection, p_visible ]( System::Visibility & p_visibility )
						{
							if ( p_visible )
							{
								p_visibility.atoms |= p_selection.atoms;
							}
							else
							{
								p_visibility.atoms &= ~p_selection.atoms;
							}
						}
					);
				}
			);
		}
	};

	struct SetVisibleItem
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::E_SYSTEM_ITEM	 p_item,
			const Core::Struct::IndexRangeList & p_ranges  = {},
			const bool							 p_visible = true
		)
		{
			switch ( p_item )
			{
			case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
				SetVisible<Core::Struct::E_SYSTEM_ITEM::SYSTEM>().execute( p_ent, p_ranges, p_visible );
				break;
			case Core::Struct::E_SYSTEM_ITEM::CHAIN:
				SetVisible<Core::Struct::E_SYSTEM_ITEM::CHAIN>().execute( p_ent, p_ranges, p_visible );
				break;
			case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
				SetVisible<Core::Struct::E_SYSTEM_ITEM::RESIDUE>().execute( p_ent, p_ranges, p_visible );
				break;
			case Core::Struct::E_SYSTEM_ITEM::ATOM:
				SetVisible<Core::Struct::E_SYSTEM_ITEM::ATOM>().execute( p_ent, p_ranges, p_visible );
				break;
			default: assert( false && "Unhandled E_SYSTEM_ITEM type in SetVisibleItem action." ); break;
			}
		}

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const Core::Struct::IndexRange &  p_range,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_range ), p_visible ); }

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const std::vector<Index> &		  p_values,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_values ), p_visible ); }

		void execute(
			const ECS::Entity				  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const Index						  p_value,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_value ), p_visible ); }
	};

} // namespace VTX::App::Action::Visibility

#endif
