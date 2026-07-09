#include "app/python_binding/application.hpp"
#include "app/action/application.hpp"

namespace VTX::App::PythonBinding
{
	void ApplicationBinder::bind( Module & p_vtxModule )
	{ p_vtxModule.bindAction<App::Action::Application::Quit>( "quit", "Quit the application." ); }
} // namespace VTX::App::PythonBinding
