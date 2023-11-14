#include "python_binding/action.hpp"
#include "python_binding/interpretor.hpp"
#include <app/vtx_app.hpp>

namespace VTX::PythonBinding::Action
{
	void RunScript::execute()
	{
		Interpretor & interpretor = App::VTXApp::get().getSystem().getSystem<Interpretor>( Interpretor::SYSTEM_KEY );
		interpretor.runScript( _path );
	}

} // namespace VTX::PythonBinding::Action
