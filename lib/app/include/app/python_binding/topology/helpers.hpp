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
	const Core::Struct::Topology & getTopology( const Entity p_entity );

	/**
	 * @brief Get the system from name first, then pdb, then file name.
	 */
	System getSystem( const std::string_view p_name );

	/**
	 * @brief Get the visible state.
	 */
	App::System::E_VISIBLE_STATE getVisibleState(
		const Entity			   p_entity,
		const SystemItem		   p_item,
		const std::optional<Index> p_index
	);
	bool isVisible( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index );
	bool isFullyVisible( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index );

	/**
	 * @brief Get the selection state.
	 */
	App::System::E_SELECTION_STATE getSelectionState(
		const Entity			   p_entity,
		const SystemItem		   p_item,
		const std::optional<Index> p_index
	);
	bool isSelected( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index );
	bool isFullySelected( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index );

} // namespace VTX::App::PythonBinding::Topology

#endif
