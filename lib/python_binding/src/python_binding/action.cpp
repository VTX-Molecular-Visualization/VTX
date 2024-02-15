#include "python_binding/action.hpp"
#include "python_binding/interpretor.hpp"
#include <app/vtx_app.hpp>

namespace VTX::PythonBinding::Action
{
	void RunScript::execute() { INTERPRETOR().runScript( _path ); }

} // namespace VTX::PythonBinding::Action
