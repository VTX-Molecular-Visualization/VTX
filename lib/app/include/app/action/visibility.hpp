#ifndef __VTX_APP_ACTION_VISIBILITY__
#define __VTX_APP_ACTION_VISIBILITY__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/selection.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/topology.hpp>
#include <util/chrono.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>
#include <utility>

namespace VTX::App::Action::Visibility
{
	inline void patchVisibility( const ECS::Entity p_ent, Util::Math::BitSet p_atoms )
	{
		auto &		 reg		= REG();
		const auto & visibility = reg.get<System::Visibility>( p_ent );
		if ( visibility.atoms == p_atoms )
		{
			return;
		}

		reg.patch<System::Visibility>(
			p_ent, [ &p_atoms ]( System::Visibility & p_visibility ) { p_visibility.atoms = std::move( p_atoms ); }
		);
	}

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

			const auto & visibility = REG().get<System::Visibility>( p_ent );

			if constexpr ( ITEM == Core::Struct::E_SYSTEM_ITEM::SYSTEM )
			{
				if ( p_visible )
				{
					if ( visibility.atoms.all() )
					{
						return;
					}

					patchVisibility( p_ent, Util::Math::BitSet( visibility.atoms.size(), true ) );
				}
				else
				{
					if ( visibility.atoms.none() )
					{
						return;
					}

					patchVisibility( p_ent, Util::Math::BitSet( visibility.atoms.size() ) );
				}
			}
			else
			{
				Core::Struct::IndexRangeList atoms	 = Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );
				Util::Math::BitSet			 current = visibility.atoms;

				if ( p_visible )
				{
					current.mergeInPlace( atoms );
				}
				else
				{
					current.subtractInPlace( atoms );
				}

				patchVisibility( p_ent, std::move( current ) );
			}
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
			REG().view<System::Selection, System::Visibility>().each(
				[ p_visible ](
					const ECS::Entity		   p_ent,
					const System::Selection &  p_selection,
					const System::Visibility & p_visibility
				)
				{
					if ( p_selection.atoms.any() )
					{
						Util::Math::BitSet current = p_visibility.atoms;
						if ( p_visible )
						{
							current.mergeInPlace( p_selection.atoms );
						}
						else
						{
							current.subtractInPlace( p_selection.atoms );
						}

						patchVisibility( p_ent, std::move( current ) );
					}
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
