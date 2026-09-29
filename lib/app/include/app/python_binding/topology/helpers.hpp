#ifndef __VTX_APP_PYTHON_BINDING_TOPOLOGY_HELPERS__
#define __VTX_APP_PYTHON_BINDING_TOPOLOGY_HELPERS__

#include "app/python_binding/topology/types.hpp"
#include <app/system/selection.hpp>
#include <app/system/visibility.hpp>
#include <optional>
#include <string>

namespace VTX::App::PythonBinding::Topology
{
	/**
	 * @brief Get the topology from entity.
	 */
	const Core::Struct::Topology & getTopology( Entity p_entity );

	/**
	 * @brief Get the system from name first, then pdb, then file name.
	 */
	System getSystem( std::string_view p_name );

	/**
	 * @brief Get the visible state.
	 */
	App::System::E_VISIBLE_STATE getVisibleState( Entity p_entity, SystemItem p_item, std::optional<Index> p_index );
	bool						 isVisible( Entity p_entity, SystemItem p_item, std::optional<Index> p_index );
	bool						 isFullyVisible( Entity p_entity, SystemItem p_item, std::optional<Index> p_index );

	/**
	 * @brief Get the selection state.
	 */
	App::System::E_SELECTION_STATE getSelectionState(
		Entity				 p_entity,
		SystemItem			 p_item,
		std::optional<Index> p_index
	);
	bool isSelected( Entity p_entity, SystemItem p_item, std::optional<Index> p_index );
	bool isFullySelected( Entity p_entity, SystemItem p_item, std::optional<Index> p_index );

} // namespace VTX::App::PythonBinding::Topology

#endif
