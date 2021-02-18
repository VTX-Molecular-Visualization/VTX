#include "io/writer/exception.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"

using namespace VTX;

int main( int p_argc, char * p_argv[] )
{
	try
	{
		VTXApp & app = VTXApp::get();
		app.start();	

		return app.exec();
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( p_e.what() );
		return EXIT_FAILURE;
	}
}
