#include <app/filesystem.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

// TODO: move to conan.
#define VTX_UI_QT
#define VTX_TOOL_EXAMPLE
#define VTX_TOOL_MDPREP

#ifdef VTX_UI_QT
#include <qt/application.hpp>
#else
#include <app/vtx_app.hpp>
#endif

#ifdef VTX_TOOL_EXAMPLE
#include <example/tool.hpp>
#endif

#ifdef VTX_TOOL_MDPREP
#include <tool/mdprep/mdprep.hpp>
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

		LOGGER::init( logDir, debug );

		std::unique_ptr<App::VTXApp> app;
#ifdef VTX_UI_QT
		if ( not args.has( "-no-gui" ) )
		{
			UI::QT::Application::configure();
			app = std::make_unique<UI::QT::Application>();
		}
		else
		{
			app = std::make_unique<App::VTXApp>();
		}
#else
		app = std::make_unique<App::VTXApp>();
		// TODO: how to create opengl context?
#endif

		assert( app != nullptr );

// Add tools.
#ifdef VTX_TOOL_EXAMPLE
		auto exampleTool = std::make_unique<Tool::Example::Tool>();
		app->addTool( exampleTool.get() );
#endif
// Add tools.
#ifdef VTX_TOOL_MDPREP
		auto mdprepTool = std::make_unique<Tool::Mdprep::MdPrep>();
		app->addTool( mdprepTool.get() );
#endif

		app->init();

		const FilePath molPath = App::Filesystem::getInternalDataDir() / "1AGA.mmtf";
		args.add( molPath.string() );

		app->start( args );

		LOGGER::stop();
		return EXIT_SUCCESS;
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		VTX_ERROR( "Unhandled exception: {}", error );
		LOGGER::stop();
		return EXIT_FAILURE;
	}
}
