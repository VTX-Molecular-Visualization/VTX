#include "util/logger.hpp"
#include "vtx_app.hpp"

using namespace VTX;

int main( int argc, char * argv[] )
{
#ifndef _DEBUG
	try
	{
#endif
		VTXApp::get().start();
#ifndef _DEBUG
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( p_e.what() );
		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}
