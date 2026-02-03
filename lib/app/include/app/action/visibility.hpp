#ifndef __VTX_APP_ACTION_VISIBILITY__
#define __VTX_APP_ACTION_VISIBILITY__

#include "app/ecs.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/system.hpp>
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
			using namespace Util::Math;
			using namespace Core::Struct;

			auto &		 reg		= REG();
			const auto & system		= reg.get<Core::Struct::System>( p_ent );
			auto &		 visibility = reg.get<System::Visibility>( p_ent );

			Core::Struct::IndexRangeList visibleAtoms = visibility.atoms;

			if constexpr ( ITEM == E_SYSTEM_ITEM::SYSTEM )
			{
				if ( p_visible )
				{
					visibleAtoms = IndexRangeList( system.getAtomRange() );
				}
				else
				{
					visibleAtoms.clear();
				}
			}
			else if constexpr ( ITEM == E_SYSTEM_ITEM::CHAIN )
			{
				if ( p_visible )
				{
					for ( const auto & index : p_ranges )
					{
						visibleAtoms.addRange( system.getChainAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						visibleAtoms.removeRange( system.getChainAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == E_SYSTEM_ITEM::RESIDUE )
			{
				if ( p_visible )
				{
					for ( const auto & index : p_ranges )
					{
						visibleAtoms.addRange( system.getResidueAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						visibleAtoms.removeRange( system.getResidueAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == E_SYSTEM_ITEM::ATOM )
			{
				if ( p_visible )
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						visibleAtoms.addRange( *it );
					}
				}
				else
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						visibleAtoms.removeRange( *it );
					}
				}
			}
			else
			{
				static_assert( always_false_v<ITEM>, "Unhandled E_ITEM type in SetVisible action." );
			}

			reg.patch<System::Visibility>(
				p_ent, [ visibleAtoms ]( System::Visibility & p_visibility ) { p_visibility.atoms = visibleAtoms; }
			);
		}

		void execute( const ECS::Entity p_ent, const Core::Struct::IndexRange & p_range, const bool p_visible = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_visible );
		}

		void execute( const ECS::Entity p_ent, const std::vector<Index> & p_values, const bool p_visible = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_visible );
		}

		void execute( const ECS::Entity p_ent, const Index p_value, const bool p_visible = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_visible );
		}
	};

	// TODO: other elements.
} // namespace VTX::App::Action::Visibility

#endif
