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
	void patchVisibility( const Entity p_ent, Util::Math::BitSet p_atoms );

	struct HideEverything
	{
		void execute();
	};

	/**
	 * @brief Set item visibility.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct SetVisible
	{
		void execute(
			const Entity						 p_ent,
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

		void execute( const Entity p_ent, const Core::Struct::IndexRange & p_range, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_visible ); }

		void execute( const Entity p_ent, const std::vector<Index> & p_values, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_visible ); }

		void execute( const Entity p_ent, const Index p_value, const bool p_visible = true )
		{ execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_visible ); }
	};

	struct SetVisibleSelected
	{
		void execute( const bool p_visible = true );
	};

	struct SetVisibleItem
	{
		void execute(
			const Entity						 p_ent,
			const Core::Struct::E_SYSTEM_ITEM	 p_item,
			const Core::Struct::IndexRangeList & p_ranges  = {},
			const bool							 p_visible = true
		);

		inline void execute(
			const Entity					  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const Core::Struct::IndexRange &  p_range,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_range ), p_visible ); }

		inline void execute(
			const Entity					  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const std::vector<Index> &		  p_values,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_values ), p_visible ); }

		inline void execute(
			const Entity					  p_ent,
			const Core::Struct::E_SYSTEM_ITEM p_item,
			const Index						  p_value,
			const bool						  p_visible = true
		)
		{ execute( p_ent, p_item, Core::Struct::IndexRangeList( p_value ), p_visible ); }
	};

} // namespace VTX::App::Action::Visibility

#endif
