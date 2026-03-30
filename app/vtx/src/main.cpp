#include <app/arguments.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

// Conditional includes.
#ifndef VTX_UI_QT
#define VTX_UI_QT 1
#endif
#if VTX_UI_QT
#include <ui/qt/application.hpp>
#else
#include <app/vtx_app.hpp>
#endif

#if VTX_TOOL_EXAMPLE
#include <tool/example/example_tool.hpp>
#endif

#if VTX_TOOL_MDPREP
#include <tool/mdprep/mdprep.hpp>
#endif

#ifdef _WIN32
#include <windows.h>
// Force high performance GPU on Windows.
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement				  = 0x00000001;
	__declspec( dllexport ) int		 AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main( int p_argc, char * p_argv[] )
{
	using namespace VTX;

	try
	{
		App::Arguments			   argss;
		std::optional<std::string> help;
		{
			App::ArgumentParser parser( p_argc, p_argv );
			parser.parse();
			if ( parser.needHelp() )
				help = parser.help();
			parser.get( argss );
		}

#ifdef _DEBUG
		argss.debug = true;
#endif

#ifdef _WIN32
		// Disable default console.
#pragma comment( linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" )
		//  Create console.
		if ( argss.debug )
		{
			AllocConsole();
			freopen_s( (FILE **)stdout, "CONOUT$", "w", stdout );
			freopen_s( (FILE **)stderr, "CONOUT$", "w", stderr );
			freopen_s( (FILE **)stdin, "CONIN$", "r", stdin );
		}
#endif

		if ( help )
		{
			std::cout << *help;
			return EXIT_SUCCESS;
		}

#if VTX_UI_QT
		if ( not argss.noGui )
		{
			// To set before QApplication construction.
			QCoreApplication::setAttribute( Qt::AA_CompressHighFrequencyEvents );

			Q_INIT_RESOURCE( vtx_qt_resources_ui );
			UI::QT::Application app( std::move( argss ) );
#if VTX_TOOL_EXAMPLE
			Q_INIT_RESOURCE( vtx_qt_resources_tool_example );
			app.addTool<Tool::Example::ExampleTool>();
#endif
#if VTX_TOOL_MDPREP
			Q_INIT_RESOURCE( vtx_qt_resources_tool_mdprep );
			app.addTool<Tool::Mdprep::MdPrep>();
#endif
			app.start();
			return EXIT_SUCCESS;
		}
#endif

		App::VTXApp app( std::move( argss ) );
#if VTX_TOOL_EXAMPLE
		app.addTool<Tool::Example::ExampleTool>();
#endif
#if VTX_TOOL_MDPREP
		app.addTool<Tool::Mdprep::MdPrep>();
#endif
		app.start();

		return EXIT_SUCCESS;
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		std::cerr << "Unhandled exception: " << error;
		return EXIT_FAILURE;
	}
}
