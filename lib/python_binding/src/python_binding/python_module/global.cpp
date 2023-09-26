#include "python_binding/binding/global.hpp"
#include <app/application/system.hpp>
#include <app/vtx_app.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <string>

int test( const int p_value ) { return p_value * 2; }

void openFile( std::shared_ptr<VTX::App::Application::System> p_system, const std::string & p_path )
{
	VTX::App::VTXApp::get().referenceSystem( p_system );
	::VTX::PythonBinding::Binding::Global::openFile( p_path );
}

PYBIND11_MODULE( PyTX, m )
{
	m.doc() = "Open file at given path. Can handle molecule files or scene files."; // optional module docstring

	pybind11::class_<VTX::App::Application::System, std::shared_ptr<VTX::App::Application::System>>( m, "VTXSystem" );

	m.def( "test", &test, "Test function.", pybind11::arg( "value" ) = 5 );
	m.def( "openFile",
		   &openFile,
		   "Open a file at given path.",
		   pybind11::arg( "system" ) = nullptr,
		   pybind11::arg( "path" )	 = "" );
}
