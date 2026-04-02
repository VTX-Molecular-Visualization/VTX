#include <app/arguments.hpp>
#include <app/session.hpp>
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

/**
 * @brief On windows, the console returns before the text is returned. The behavior is tight to the Windows OS : a
 * console starts a new process and returns, then the process writes stuff in the console. It makes the help text appear
 * after where the user expect the cursor to be, making it look like it is stuck even though it is not.
 * To work around this, we add an "enter" input after we flush the stdcout to have a fresh cursor after the help text.
 * All this stuff is pointless on linux as the console waits for the process to finish before returning hand.
 */
void unblockParentConsole() noexcept
{
#ifdef _WIN32
	std::cout.flush();
	// Inject Enter to unblock the cmd.exe prompt that fired before our output.
	INPUT_RECORD ir						= {};
	ir.EventType						= KEY_EVENT;
	ir.Event.KeyEvent.bKeyDown			= TRUE;
	ir.Event.KeyEvent.wRepeatCount		= 1;
	ir.Event.KeyEvent.wVirtualKeyCode	= VK_RETURN;
	ir.Event.KeyEvent.wVirtualScanCode	= MapVirtualKey( VK_RETURN, MAPVK_VK_TO_VSC );
	ir.Event.KeyEvent.uChar.UnicodeChar = L'\r';

	DWORD written;
	WriteConsoleInput( GetStdHandle( STD_INPUT_HANDLE ), &ir, 1, &written );
#endif
}

int main( int p_argc, char * p_argv[] )
{
	using namespace VTX;

	try
	{
		App::Session::handleStartupActivation();

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
#pragma comment( linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup" )
		// Attach to parent console if launched from CLI (cmd/PowerShell).
		// Falls through silently if launched from Explorer (no parent console).
		bool hasParentConsole = AttachConsole( ATTACH_PARENT_PROCESS );
		if ( not hasParentConsole && argss.debug )
			AllocConsole();
		if ( hasParentConsole || argss.debug )
		{
			freopen_s( (FILE **)stdout, "CONOUT$", "w", stdout );
			freopen_s( (FILE **)stderr, "CONOUT$", "w", stderr );
			freopen_s( (FILE **)stdin, "CONIN$", "r", stdin );
		}
#endif

		if ( help )
		{
			std::cout << std::endl << *help;
#ifdef _WIN32
			if ( hasParentConsole )
				unblockParentConsole();
#endif
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
