#ifndef __VTX_APP_PYTHON_BINDING_TOPOLOGY_ACTIONS__
#define __VTX_APP_PYTHON_BINDING_TOPOLOGY_ACTIONS__

#include "app/action/selection.hpp"
#include "app/action/visibility.hpp"
#include "app/python_binding/module.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include "app/python_binding/topology/types.hpp"

namespace VTX::App::PythonBinding::Topology
{
	/**
	 * @brief Set the visibility from App::Action.
	 */
	template<SystemItem ITEM>
	inline void setVisible( const Entity p_entity, const RangeList & p_ranges, const bool p_visible )
	{
		getTopology( p_entity );

		executeAction<App::Action::Visibility::SetVisible<ITEM>>( p_entity, p_ranges, p_visible );
	}

	inline void setVisible(
		const Entity	  p_entity,
		const SystemItem  p_item,
		const RangeList & p_ranges,
		const bool		  p_visible
	)
	{
		getTopology( p_entity );

		executeAction<App::Action::Visibility::SetVisibleItem>( p_entity, p_item, p_ranges, p_visible );
	}

	/**
	 * @brief Set selected from App::Action.
	 */
	template<SystemItem ITEM>
	inline void setSelected(
		const Entity	  p_entity,
		const RangeList & p_ranges,
		const bool		  p_selected,
		const bool		  p_append
	)
	{
		getTopology( p_entity );

		executeAction<App::Action::Selection::SetSelected<ITEM>>( p_entity, p_ranges, p_selected, p_append );
	}

	inline void setSelected(
		const Entity	  p_entity,
		const SystemItem  p_item,
		const RangeList & p_ranges,
		const bool		  p_selected,
		const bool		  p_append
	)
	{
		getTopology( p_entity );

		executeAction<App::Action::Selection::SetSelectedItem>( p_entity, p_item, p_ranges, p_selected, p_append );
	}
} // namespace VTX::App::PythonBinding::Topology

#endif
