#include "binding.hpp"
#include "action.hpp"
#include <python_binding/binder.hpp>
#include <python_binding/wrapper/module.hpp>

namespace VTX::Test::ExternalTool
{
	void Binder::bind( PythonBinding::PyTXModule & p_vtxModule )
	{
		p_vtxModule.commands().bindAction<Action::ToolAction>( "new_command", "Invoke a new command function" );
	}
}; // namespace VTX::Test::ExternalTool
