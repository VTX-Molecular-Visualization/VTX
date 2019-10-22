#include "util/logger.hpp"
#include "vtx_app.hpp"

using namespace VTX;

int main( int argc, char * argv[] )
{
	try
	{
		VTXApp & app = VTXApp::INSTANCE();

		app.start();
		app.stop();
	}
	catch ( const std::exception & e )
	{
		ERROR( e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
