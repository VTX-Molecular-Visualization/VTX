#include "binding.hpp"
#include "action.hpp"
#include <python_binding/binder.hpp>
#include <python_binding/wrapper/module.hpp>

namespace VTX::Test::ExternalTool
{
	void Binder::bind( PythonBinding::Wrapper::Module & p_vtxModule )
	{
		PythonBinding::Wrapper::Module commandModule = p_vtxModule.getSubmodule( "Command" );
		commandModule.bindAction<Action::ToolAction>( "new_command", "Invoke a new command function" );
	}
}; // namespace VTX::Test::ExternalTool
