#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/selection.hpp"
#include <core/struct/system.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Selection
{

	/**
	 * @brief Set item selection.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct SetSelected
	{
		void execute(
			const ECS::Entity					 p_ent,
			const Core::Struct::IndexRangeList & p_ranges	= {},
			const bool							 p_selected = true,
			const bool							 p_append	= false
		)
		{
			Util::ScopedChrono timer( "App::Action::SetSelected" );

			auto &						 reg	   = REG();
			auto &						 selection = reg.get<System::Selection>( p_ent );
			Core::Struct::IndexRangeList atoms	   = Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );
			Core::Struct::IndexRangeList current   = selection.atoms;

			if ( not p_append )
			{
				current.clear();
			}
			if ( p_selected )
			{
				current.mergeInPlace( atoms );
			}
			else
			{
				current.substractInPlace( atoms );
			}

			if ( selection.atoms != current )
			{
				// VTX_DEBUG( "{} {}", selection.atoms.toString(), current.toString() );
				reg.patch<System::Selection>(
					p_ent, [ &current ]( System::Selection & p_selection ) { p_selection.atoms = current; }
				);
			}
		}

		inline void execute(
			const ECS::Entity				 p_ent,
			const Core::Struct::IndexRange & p_range,
			const bool						 p_selected = true,
			const bool						 p_append	= false
		)
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_selected, p_append );
		}

		inline void execute(
			const ECS::Entity		   p_ent,
			const std::vector<Index> & p_values,
			const bool				   p_selected = true,
			const bool				   p_append	  = false
		)
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_selected, p_append );
		}

		inline void execute(
			const ECS::Entity p_ent,
			const Index		  p_value,
			const bool		  p_selected = true,
			const bool		  p_append	 = false
		)
		{
			execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_selected, p_append );
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
		 * @brief Clear mode.
		 */
		enum struct E_MODE : uint
		{
			THIS,
			BUT
		};

		/**
		 * @brief For a specific system.
		 */
		void execute( const ECS::Entity, const E_MODE );
	};

	enum struct E_GRANULARITY : uint
	{
		ATOM,
		RESIDUE,
		CHAIN,
		SYSTEM
	};

	/**
	 * @brief Pick item at mouse position.
	 */
	struct Pick
	{
		void execute( const Vec2i &, const E_GRANULARITY = E_GRANULARITY::RESIDUE, const bool = false );
	};
} // namespace VTX::App::Action::Selection

#endif
