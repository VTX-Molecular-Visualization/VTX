#include "app/python_binding/visibility.hpp"
#include "app/action/visibility.hpp"
#include "app/ecs.hpp"
#include "app/helper/system.hpp"
#include "app/system/visibility.hpp"
#include <core/struct/topology.hpp>
#include <optional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <python_binding/binding/entity_caster.hpp>
#include <python_binding/binding/helper.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <vector>

namespace VTX::App::PythonBinding
{
	namespace
	{
		using SystemItem = Core::Struct::E_SYSTEM_ITEM;
		using IndexList	 = std::vector<Index>;

		App::System::E_VISIBLE_STATE _getVisibleState(
			const Entity			   p_ent,
			const SystemItem		   p_item,
			const std::optional<Index> p_index = std::nullopt
		)
		{
			if ( not REG().all_of<Core::Struct::Topology, App::System::Visibility>( p_ent ) )
			{
				throw pybind11::value_error( "Invalid system id." );
			}

			return Helper::System::getVisibleState( { p_ent, p_item, p_index } );
		}
	} // namespace

	void VisibilityBinder::bind( Module & p_vtxModule )
	{
		pybind11::module_ * module = nullptr;
		p_vtxModule.getPythonModule( &module );

		VTX::PythonBinding::Helper::declareEnum<App::System::E_VISIBLE_STATE>( *module, "VISIBLE_STATE" );

		p_vtxModule.def(
			"showAll",
			[]() { executeAction<App::Action::Visibility::SetVisibleAll>( true ); },
			"Show all loaded systems."
		);
		p_vtxModule.def(
			"hideAll",
			[]() { executeAction<App::Action::Visibility::SetVisibleAll>( false ); },
			"Hide all loaded systems."
		);
		p_vtxModule.bindAction<App::Action::Visibility::SetVisibleSelected, const bool>(
			"setVisibleSelected",
			"Set visibility of the current selection.",
			VTX::PythonBinding::Wrapper::VArg<bool>( "visible", true )
		);
		p_vtxModule.def(
			"setVisible",
			[]( const Entity p_ent, const SystemItem p_item, const IndexList & p_indices, const bool p_visible )
			{
				if ( not REG().all_of<Core::Struct::Topology, App::System::Visibility>( p_ent ) )
				{
					throw pybind11::value_error( "Invalid system id." );
				}

				executeAction<App::Action::Visibility::SetVisibleItem>( p_ent, p_item, p_indices, p_visible );
			},
			"Set item visibility for a system.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<IndexList>( "indices", IndexList() ),
			VTX::PythonBinding::Wrapper::VArg<bool>( "visible", true )
		);
		p_vtxModule.def(
			"getVisibleState",
			&_getVisibleState,
			"Return the visibility state of an item.",
			VTX::PythonBinding::Wrapper::Arg( "systemId" ),
			VTX::PythonBinding::Wrapper::VArg<SystemItem>( "item", SystemItem::SYSTEM ),
			VTX::PythonBinding::Wrapper::VArg<std::optional<Index>>( "index", std::optional<Index>() )
		);
	}
} // namespace VTX::App::PythonBinding
