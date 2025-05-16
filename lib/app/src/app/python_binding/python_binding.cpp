
#include "app/python_binding/python_binding.hpp"
#include "app/action/application.hpp"
#include "app/python_binding/selection/binder.hpp"
#include <python_binding/action.hpp>
#include <python_binding/binder.hpp>
#include <python_binding/binding/vtx_module.hpp>
#include <python_binding/wrapper/arg.hpp>
#include <util/logger.hpp>

namespace VTX::App::PythonBinding
{
	void VTXAppBinder::bind( VTX::PythonBinding::PyTXModule & p_vtxmodule )
	{
		VTX::PythonBinding::Wrapper::Module commands = p_vtxmodule.commands();
		VTX::VTX_INFO( "Applying binding on module." );
		commands.bindAction<App::Action::Application::Open, const std::string &>(
			"openFile", "Open files at given path.", VTX::PythonBinding::Wrapper::Arg( "path" )
		);

		commands.bindAction<App::Action::Application::NewScene>( "newScene", "Clear scene." );
		commands.bindAction<App::Action::Application::OpenScene, const std::string &>(
			"openScene", "Open scene at given path.", VTX::PythonBinding::Wrapper::Arg( "path" )
		);
		commands.bindAction<App::Action::Application::SaveScene, const std::string &>(
			"save", "Save scene.", VTX::PythonBinding::Wrapper::VArg<std::string>( "path", "" )
		);
		commands.bindAction<App::Action::Application::ClearScene>( "clear", "Clear scene." );

		commands.bindAction<App::Action::Application::SaveSettings>( "saveSettings", "Save settings." );
		commands.bindAction<App::Action::Application::ReloadSettings>( "reloadSettings", "Reload settings." );
		commands.bindAction<App::Action::Application::ResetSettings>( "resetSettings", "Reset settings." );

		pybind11::module_ * apiModulePtr = nullptr;
		p_vtxmodule.api().getPythonModule( &apiModulePtr );
		Selection::bind_selection( *apiModulePtr );
	}

	void VTXAppBinder::importHeaders()
	{
		VTX::PythonBinding::importObject( fmt::format( "{}.API", VTX::PythonBinding::vtx_module_name() ), "select" );
		VTX::PythonBinding::importObject( fmt::format( "{}.API", VTX::PythonBinding::vtx_module_name() ), "intersect" );
		VTX::PythonBinding::importObject( fmt::format( "{}.API", VTX::PythonBinding::vtx_module_name() ), "exclusive" );
	}

} // namespace VTX::App::PythonBinding
