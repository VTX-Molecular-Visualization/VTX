#include "io/writer/writer_exception.hpp"
#include "tool/logger.hpp"
#include "ui/dialog.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#ifdef _WIN32
#include "wtypes.h"
#endif

using namespace VTX;

#ifdef _WIN32
// Force GPU (NVIDIA/AMD).
extern "C"
{
	__declspec( dllexport ) DWORD NvOptimusEnablement				 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#ifdef VTX_PRODUCTION
// Hide console.
#pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
#endif
#endif

int main( int p_argc, char * p_argv[] )
{
	try
	{
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		VTXApp & app = VTXApp::get();

		const std::vector<std::string> args( p_argv, p_argv + p_argc );
		app.start( std::vector( args.begin() + 1, args.end() ) );

		return app.exec();
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( p_e.what() );
#ifdef VTX_PRODUCTION
		UI::Dialog::unhandledException();
#else
		return EXIT_FAILURE;
#endif
	}
}
