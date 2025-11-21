#ifndef __VTX_APP_ACTION_VISIBILITY__
#define __VTX_APP_ACTION_VISIBILITY__

#include "app/ecs.hpp"
#include "app/scene/root.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/system.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Visibility
{
	/**
	 * @brief Set item visibility.
	 */
	template<App::Scene::E_ITEM ITEM>
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

			const auto & system		= REG().get<Core::Struct::System>( p_ent );
			auto &		 visibility = REG().get<System::Visibility>( p_ent );

			if constexpr ( ITEM == App::Scene::E_ITEM::SYSTEM )
			{
				if ( p_visible )
				{
					visibility.atoms = IndexRangeList( system.getAtomRange() );
				}
				else
				{
					visibility.atoms.clear();
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::CHAIN )
			{
				if ( p_visible )
				{
					for ( const auto & index : p_ranges )
					{
						visibility.atoms.addRange( system.getChainAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						visibility.atoms.removeRange( system.getChainAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::RESIDUE )
			{
				if ( p_visible )
				{
					for ( const auto & index : p_ranges )
					{
						visibility.atoms.addRange( system.getResidueAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						visibility.atoms.removeRange( system.getResidueAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == App::Scene::E_ITEM::ATOM )
			{
				if ( p_visible )
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						visibility.atoms.addRange( *it );
					}
				}
				else
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						visibility.atoms.removeRange( *it );
					}
				}
			}
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
