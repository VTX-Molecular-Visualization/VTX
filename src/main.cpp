#include "generic/base_opengl.hpp"
#include "tool/logger.hpp"
#include "ui/dialog.hpp"
#include "vtx_app.hpp"
#include <QSurfaceFormat>
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
		std::cout << "Starting" << std::endl;
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
		QSurfaceFormat format;
		format.setVersion( OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( int( VTX_SETTING().ACTIVE_VSYNC_DEFAULT ) );
		QSurfaceFormat::setDefaultFormat( format );

		// Create application.
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
