#include "python_binding/binding/vtx_app_binder.hpp"
#include "python_binding/action.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/binding/vtx_module.hpp"
#include "python_binding/wrapper/arg.hpp"
#include <util/logger.hpp>

namespace VTX::PythonBinding::Binding
{
	void VTXAppBinder::bind( PyTXModule & p_vtxmodule )
	{
#ifdef JEVEUPAS

		Wrapper::Module commands = p_vtxmodule.commands();
		VTX::VTX_INFO( "Applying binding on module." );
		commands.bindAction<App::Action::Application::Open, const std::string &>(
			"openFile", "Open files at given path.", Wrapper::Arg( "path" )
		);

		commands.bindAction<App::Action::Application::NewScene>( "newScene", "Clear scene." );
		commands.bindAction<App::Action::Application::OpenScene, const std::string &>(
			"openScene", "Open scene at given path.", Wrapper::Arg( "path" )
		);
		commands.bindAction<App::Action::Application::SaveScene, const std::string &>(
			"save", "Save scene.", Wrapper::VArg<std::string>( "path", "" )
		);
		commands.bindAction<App::Action::Application::ClearScene>( "clear", "Clear scene." );

		commands.bindAction<App::Action::Application::SaveSettings>( "saveSettings", "Save settings." );
		commands.bindAction<App::Action::Application::ReloadSettings>( "reloadSettings", "Reload settings." );
		commands.bindAction<App::Action::Application::ResetSettings>( "resetSettings", "Reset settings." );

		commands.bindAction<PythonBinding::Action::RunScript, const std::string &>(
			"runScript", "Run a Python script at given path.", Wrapper::Arg( "path" )
		);
#endif // JEVEUPAS
	}

	void VTXAppBinder::importHeaders()
	{
		_importObject( fmt::format( "{}.API", vtx_module_name() ), "select" );
		_importObject( fmt::format( "{}.API", vtx_module_name() ), "intersect" );
		_importObject( fmt::format( "{}.API", vtx_module_name() ), "exclusive" );
	}

} // namespace VTX::PythonBinding::Binding
