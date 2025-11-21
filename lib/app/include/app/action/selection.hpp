#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/ecs.hpp"
#include "app/scene/root.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include <core/struct/system.hpp>
#include <util/event_hub.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Selection
{
	/**
	 * @brief Set item selection.
	 */
	template<App::Scene::E_ITEM ITEM>
	struct SetSelected
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::IndexRangeList & p_ranges	= {},
			const bool							 p_selected = true
		)
		{
			using namespace Util::Math;
			using namespace Core::Struct;

			const auto & system	   = REG().get<Core::Struct::System>( p_ent );
			auto &		 selection = REG().get<System::Selection>( p_ent );

			if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
			{
				if ( p_selected )
				{
					selection.atoms = IndexRangeList( system.getAtomRange() );
				}
				else
				{
					selection.atoms.clear();
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
			{
				if ( p_selected )
				{
					for ( const auto & index : p_ranges )
					{
						selection.atoms.addRange( system.getChainAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						selection.atoms.removeRange( system.getChainAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
			{
				if ( p_selected )
				{
					for ( const auto & index : p_ranges )
					{
						selection.atoms.addRange( system.getResidueAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						selection.atoms.removeRange( system.getResidueAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
			{
				if ( p_selected )
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						selection.atoms.addRange( *it );
					}
				}
				else
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						selection.atoms.removeRange( *it );
					}
				}
			}

			HUB().trigger<Events::SelectionChange>( { p_ent } );
		}

		void execute( const ECS::Entity p_ent, const Core::Struct::IndexRange & p_range, const bool p_selected = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_selected );
		}

		void execute( const ECS::Entity p_ent, const std::vector<Index> & p_values, const bool p_selected = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_selected );
		}

		void execute( const ECS::Entity p_ent, const Index p_value, const bool p_selected = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_selected );
		}
	};
} // namespace VTX::App::Action::Selection

#endif
