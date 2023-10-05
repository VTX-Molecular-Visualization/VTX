#include "python_binding/binding/global.hpp"
#include "python_binding/log_redirection.hpp"
#include <app/application/system.hpp>
#include <app/vtx_app.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <string>

namespace VTX
{
	void _init( std::shared_ptr<App::Application::System> p_system ) { App::VTXApp::get().referenceSystem( p_system ); }
} // namespace VTX

using namespace VTX;
using namespace VTX::PythonBinding::Binding;

PYBIND11_MODULE( PyTX, m )
{
	m.doc() = "Open file at given path. Can handle molecule files or scene files."; // optional module docstring

	// Global pointer to VTX data
	pybind11::class_<App::Application::System, std::shared_ptr<App::Application::System>>( m, "VTXSystem" );

	// Class to redirect Python prints
	pybind11::class_<PythonBinding::LogRedirection>( m, "LogRedirection" )
		.def( "write", &PythonBinding::LogRedirection::write )
		.def( "flush", &PythonBinding::LogRedirection::flush );

	// Core module : Contains some core functions which must be hidden for users
	pybind11::module_ vtxCoreModule = m.def_submodule( "Core", "VTX Python core functions" );
	vtxCoreModule.doc()				= "Contains some core functions which must be hidden for users.";

	vtxCoreModule.def( "_init", &_init, "Initialize python environment for commands" );

	// Command module : Contains all commands accessible to user via command line.
	pybind11::module_ vtxCommandModule = m.def_submodule( "Command", "VTX Python command interface" );
	vtxCommandModule.doc()			   = "Command module : Contains all commands accessible to user via command line.";

	vtxCommandModule.def( "openFile", &Global::openFile, "Open a file at given path.", pybind11::arg( "path" ) = "" );
	vtxCommandModule.def(
		"runScript", &Global::runScript, "Run a Python script at given path.", pybind11::arg( "path" ) = "" );
}
