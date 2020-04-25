#include "util/logger.hpp"
#include "vtx_app.hpp"

using namespace VTX;

int main( int argc, char * argv[] )
{
	try
	{
		VTXApp::get().start();
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( p_e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
