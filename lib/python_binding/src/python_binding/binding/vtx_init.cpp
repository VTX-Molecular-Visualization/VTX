#include "python_binding/binding/vtx_init.hpp"
#include "python_binding/action.hpp"
#include "python_binding/binder.hpp"
#include "python_binding/wrapper/arg.hpp"
#include <app/internal/action/ecs.hpp>

namespace VTX::PythonBinding::Binding
{
	void VTXAppBinder::bind( Wrapper::Module & p_vtxmodule )
	{
		using namespace VTX::App;

		Wrapper::Module commands = p_vtxmodule.getSubmodule( "Command" );

		commands.bindAction<Internal::Action::ECS::Open, const std::string &>(
			"openFile", "Open a file at given path.", Wrapper::Arg( "path" ) );

		commands.bindAction<PythonBinding::Action::RunScript, const std::string &>(
			"runScript", "Run a Python script at given path.", Wrapper::Arg( "path" ) );
	}
} // namespace VTX::PythonBinding::Binding
