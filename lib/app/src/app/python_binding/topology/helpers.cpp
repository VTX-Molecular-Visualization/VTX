#include "app/python_binding/topology/helpers.hpp"
#include "app/helper/system.hpp"
#include <pybind11/pybind11.h>

namespace VTX::App::PythonBinding::Topology
{
	namespace py = pybind11;

	const Core::Struct::Topology & getTopology( const Entity p_entity )
	{
		if ( p_entity == InvalidEntity || not REG().all_of<Core::Struct::Topology>( p_entity ) )
		{
			throw py::value_error( "Invalid system" );
		}

		return REG().get<Core::Struct::Topology>( p_entity );
	}

	System getSystem( const std::string_view p_name )
	{
		Entity entity = Helper::System::getSystemByName( p_name );
		if ( entity == InvalidEntity )
		{
			entity = Helper::System::getSystemByPdb( p_name );
		}
		if ( entity == InvalidEntity )
		{
			entity = Helper::System::getSystemByFileName( p_name );
		}

		getTopology( entity );
		return { entity };
	}

	App::System::E_VISIBLE_STATE getVisibleState(
		const Entity			   p_entity,
		const SystemItem		   p_item,
		const std::optional<Index> p_index
	)
	{
		getTopology( p_entity );

		return Helper::System::getVisibleState( { p_entity, p_item, p_index } );
	}

	App::System::E_SELECTION_STATE getSelectionState(
		const Entity			   p_entity,
		const SystemItem		   p_item,
		const std::optional<Index> p_index
	)
	{
		getTopology( p_entity );

		return Helper::System::getSelectionState( { p_entity, p_item, p_index } );
	}

	bool isVisible( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index )
	{
		return getVisibleState( p_entity, p_item, p_index ) != App::System::E_VISIBLE_STATE::HIDDEN;
	}

	bool isFullyVisible( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index )
	{
		return getVisibleState( p_entity, p_item, p_index ) == App::System::E_VISIBLE_STATE::VISIBLE;
	}

	bool isSelected( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index )
	{
		return getSelectionState( p_entity, p_item, p_index ) != App::System::E_SELECTION_STATE::NONE;
	}

	bool isFullySelected( const Entity p_entity, const SystemItem p_item, const std::optional<Index> p_index )
	{
		return getSelectionState( p_entity, p_item, p_index ) == App::System::E_SELECTION_STATE::FULL;
	}
} // namespace VTX::App::PythonBinding::Topology
