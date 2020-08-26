#include "StackWalker.h"
#include "io/writer/exception.hpp"
#include "util/filesystem.hpp"
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

		IO::Writer::Exception writer = IO::Writer::Exception();
		Path				  path	 = Util::Filesystem::getLogsPath( Util::Time::getTimestamp() + ".crash.log" );
		writer.writeFile( path, p_e );

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
