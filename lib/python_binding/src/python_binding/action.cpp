#include "python_binding/action.hpp"
#include "python_binding/interpretor.hpp"

namespace VTX::PythonBinding::Action
{
	void RunScript::execute()
	{
		Interpretor interpretor = Interpretor();
		interpretor.runScript( _path );
	}

} // namespace VTX::PythonBinding::Action
