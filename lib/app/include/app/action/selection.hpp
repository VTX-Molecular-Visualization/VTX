#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/selection.hpp"
#include <core/struct/topology.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>
#include <utility>

namespace VTX::App::Action::Selection
{
	/**
	 * @brief Patch selection if different from current selection to avoid unnecessary updates.
	 */
	inline void patchSelection( const Entity p_ent, Util::Math::BitSet p_atoms )
	{
		auto &		 reg	   = REG();
		const auto & selection = reg.get<System::Selection>( p_ent );
		if ( selection.atoms == p_atoms )
		{
			return;
		}

		reg.patch<System::Selection>(
			p_ent, [ &p_atoms ]( System::Selection & p_selection ) { p_selection.atoms = std::move( p_atoms ); }
		);
	}

	/**
	 * @brief Set item selection.
	 */
	template<Core::Struct::E_SYSTEM_ITEM ITEM>
	struct SetSelected
	{
		void execute(
			const Entity					 p_ent,
			const Core::Struct::IndexRangeList & p_ranges	= {},
			const bool							 p_selected = true,
			const bool							 p_append	= false
		)
		{
			Util::ScopedChrono timer( "App::Action::SetSelected" );

			const auto &				 selection = REG().get<System::Selection>( p_ent );
			Core::Struct::IndexRangeList atoms	   = Helper::System::getAtomRangeList<ITEM>( p_ent, p_ranges );
			Util::Math::BitSet			 current   = selection.atoms;

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
				current.subtractInPlace( atoms );
			}

			patchSelection( p_ent, std::move( current ) );
		}

		inline void execute(
			const Entity				 p_ent,
			const Core::Struct::IndexRange & p_range,
			const bool						 p_selected = true,
			const bool						 p_append	= false
		)
		{ execute( p_ent, Core::Struct::IndexRangeList( p_range ), p_selected, p_append ); }

		inline void execute(
			const Entity		   p_ent,
			const std::vector<Index> & p_values,
			const bool				   p_selected = true,
			const bool				   p_append	  = false
		)
		{ execute( p_ent, Core::Struct::IndexRangeList( p_values ), p_selected, p_append ); }

		inline void execute(
			const Entity p_ent,
			const Index		  p_value,
			const bool		  p_selected = true,
			const bool		  p_append	 = false
		)
		{ execute( p_ent, Core::Struct::IndexRangeList( p_value ), p_selected, p_append ); }
	};

	struct SelectAll
	{
		void execute();
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
			OTHERS
		};

		/**
		 * @brief For a specific system.
		 */
		void execute( const Entity, const E_MODE );
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
