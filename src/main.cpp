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
extern "C"
{
	__declspec( dllexport ) DWORD NvOptimusEnablement				 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#ifdef VTX_PRODUCTION
#pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
#endif
#endif

int main( int p_argc, char * p_argv[] )
{
	try
	{
		QCoreApplication::setAttribute( Qt::ApplicationAttribute::AA_EnableHighDpiScaling );
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		// QGuiApplication::setHighDpiScaleFactorRoundingPolicy( Qt::HighDpiScaleFactorRoundingPolicy::Ceil );
		VTXApp & app = VTXApp::get();
		app.start();

		return app.getQApp().exec();
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
