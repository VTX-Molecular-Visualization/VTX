#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/action/action_manager.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/system/selection.hpp"
#include <core/struct/system.hpp>
#include <util/event_hub.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Selection
{

	/**
	 * @brief Set item selection.
	 */
	template<Scene::E_ITEM ITEM>
	struct SetSelected
	{
		void execute(
			const ECS::Entity p_ent,

			const Core::Struct::IndexRangeList & p_ranges	= {},
			const bool							 p_selected = true
		)
		{
			using namespace Util::Math;
			using namespace Core::Struct;

			auto &		 reg	   = REG();
			const auto & system	   = reg.get<Core::Struct::System>( p_ent );
			const auto & selection = reg.get<System::Selection>( p_ent );

			Core::Struct::IndexRangeList selectionAtoms = selection.atoms;

			if constexpr ( ITEM == Scene::E_ITEM::SYSTEM )
			{
				if ( p_selected )
				{
					selectionAtoms = IndexRangeList( system.getAtomRange() );
				}
				else
				{
					selectionAtoms.clear();
				}
			}
			else if constexpr ( ITEM == Scene::E_ITEM::CHAIN )
			{
				if ( p_selected )
				{
					for ( const auto & index : p_ranges )
					{
						selectionAtoms.addRange( system.getChainAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						selectionAtoms.removeRange( system.getChainAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == Scene::E_ITEM::RESIDUE )
			{
				if ( p_selected )
				{
					for ( const auto & index : p_ranges )
					{
						selectionAtoms.addRange( system.getResidueAtomRange( index ) );
					}
				}
				else
				{
					for ( const auto & index : p_ranges )
					{
						selectionAtoms.removeRange( system.getResidueAtomRange( index ) );
					}
				}
			}
			else if constexpr ( ITEM == Scene::E_ITEM::ATOM )
			{
				if ( p_selected )
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						selectionAtoms.addRange( *it );
					}
				}
				else
				{
					for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); it++ )
					{
						selectionAtoms.removeRange( *it );
					}
				}
			}
			else
			{
				static_assert( always_false_v<ITEM>, "Unsupported Scene::E_ITEM type in SetSelected action." );
			}

			reg.patch<System::Selection>(
				p_ent, [ selectionAtoms ]( System::Selection & p_selection ) { p_selection.atoms = selectionAtoms; }
			);
		}

		inline void execute(
			const ECS::Entity				 p_ent,
			const Core::Struct::IndexRange & p_range,
			const bool						 p_selected = true
		)
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_selected );
		}

		inline void execute(
			const ECS::Entity		   p_ent,
			const std::vector<Index> & p_values,
			const bool				   p_selected = true
		)
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_selected );
		}

		inline void execute( const ECS::Entity p_ent, const Index p_value, const bool p_selected = true )
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_selected );
		}
	};

	/**
	 * @brief Clear selection.
	 */
	struct Clear
	{
		/**
		 * @brief For all systems.
		 */
		void execute();

		/**
		 * @brief For a specific system.
		 */
		void execute( const ECS::Entity p_ent );
	};

	/**
	 * @brief Pick item at mouse position.
	 */
	struct Pick
	{
		enum class E_GRANULARITY
		{
			ATOM,
			RESIDUE,
			CHAIN,
			SYSTEM
		};

		void execute( const Vec2i &, const E_GRANULARITY = E_GRANULARITY::ATOM, const bool = false );
	};
} // namespace VTX::App::Action::Selection

#endif
