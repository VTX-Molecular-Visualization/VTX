#include "util/logger.hpp"
#include "vtx_app.hpp"

using namespace VTX;

int main( int argc, char * argv[] )
{
	try
	{
		VTXApp::INSTANCE().start();
	}
	catch ( const std::exception & e )
	{
		ERROR( e.what() );
		VTXApp::INSTANCE().stop();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
