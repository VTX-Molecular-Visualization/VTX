// #include <app/internal/action/ecs.hpp>
#include <pybind11/pybind11.h>
// #include <string>

// void open( const std::string & p_path )
//{
//	Internal::Action::ECS::Open openAction = Internal::Action::ECS::Open( p_path );
//	openAction.execute();
// }

int test( const int p_value ) { return p_value * 2; }

// void open( const std::string & p_path ) {}

PYBIND11_MODULE( PyTX, m )
{
	m.doc() = "Open file at given path. Can handle molecule files or scene files."; // optional module docstring
	m.def( "test", &test, "Open a file at given path", pybind11::arg( "value" ) = 1 );
}
