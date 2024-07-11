#include <app/filesystem.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

// TODO: move to conan.
#define VTX_UI_QT

#ifdef VTX_UI_QT
#include <qt/application.hpp>
#else
#endif

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement				  = 0x00000001;
	__declspec( dllexport ) int		 AmdPowerXpressRequestHighPerformance = 1;
}
#endif

//  Hide console.
// #ifdef VTX_PRODUCTION
// #pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
// #endif

int main( int p_argc, char * p_argv[] )
{
	//::ShowWindow( ::GetConsoleWindow(), SW_HIDE );
	using namespace VTX;

	try
	{
		App::Args args( p_argc, p_argv );

		const FilePath logDir = VTX::App::Filesystem::getLogsDir();

		bool debug = args.has( "-debug" );
#ifdef _DEBUG
		debug = true;
#endif

		LOGGER().init( logDir, debug );

		// std::unique_ptr<App::VTXApp> app;
		App::VTXApp * app;
#ifdef VTX_UI_QT
		if ( not args.has( "-no-gui" ) )
		{
			// app = std::make_unique<VTX::UI::QT::Application>();
			app = &APP_QT();
		}
		else
		{
			app = &APP();
			// app = std::unique_ptr<App::VTXApp>( &APP() );
		}
#else
		app = std::unique_ptr<App::VTXApp>( &APP() );
#endif

		// std::unique_ptr<VTX::UI::Core::BaseUIApplication> vtxApplication = UI::UIGenerator::createUI();
		// VTX::UI::Environment::get().setUIApp( vtxApplication.get() );
		// vtxApplication->init();

		// const std::vector<std::string> args( p_argv, p_argv + p_argc );
		// vtxApplication->start( std::vector( args.begin() + 1, args.end() ) );

		const FilePath molPath = App::Filesystem::getInternalDataDir() / "md_0_1.gro";
		// args.add( molPath.string() );

		app->start( args );

		LOGGER().stop();
		return 0;
		// vtxApplication->getReturnCode();
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		VTX_ERROR( "{}", error );
		LOGGER().stop();
		return EXIT_FAILURE;
	}
}
