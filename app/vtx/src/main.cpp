#include <app/filesystem.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

// Conditional includes.
#define VTX_UI_QT 1
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
		App::Args args( p_argc, p_argv );

#ifdef _DEBUG
		args.add( App::ARG_DEBUG );
#endif
		bool debug = args.has( App::ARG_DEBUG );

#ifdef _WIN32
		//  Hide console.
		if ( not debug )
		{
			// FreeConsole();
		}
#endif

		LOGGER::init( VTX::App::Filesystem::getLogsDir(), debug );

		std::unique_ptr<App::VTXApp> app;
#if VTX_UI_QT
		if ( not args.has( App::ARG_NO_GUI ) )
		{
			// To set before QApplication construction.
			QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
			QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
			Q_INIT_RESOURCE( vtx_qt_resources_ui );
			app = std::make_unique<UI::QT::Application>( args );
		}
		else
		{
			app = std::make_unique<App::VTXApp>( args );
		}
#else
		app = std::make_unique<App::VTXApp>();
#endif

		assert( app != nullptr );

// Add tools.
#if VTX_TOOL_EXAMPLE
		auto exampleTool = std::make_unique<Tool::Example::ExampleTool>();
		app->addTool( exampleTool.get() );
#if VTX_UI_QT
		Q_INIT_RESOURCE( vtx_qt_resources_tool_example );
#endif
#endif
// Add tools.
#if VTX_TOOL_MDPREP
		auto mdprepTool = std::make_unique<Tool::Mdprep::MdPrep>();
		app->addTool( mdprepTool.get() );
#if VTX_UI_QT
		Q_INIT_RESOURCE( vtx_qt_resources_tool_mdprep );
#endif
#endif

		// const FilePath molPath = App::Filesystem::getInternalDataDir() / "1AGA.mmtf";
		// const FilePath molPath = "1AGA";
		// args.add( molPath.string() );
		const std::string	moleculeName	 = "2ama_1_npt";
		const std::string	moleculePathname = moleculeName + ".trr";
		const VTX::FilePath moleculePath	 = VTX::Util::Filesystem::getExecutableDir() / "data\\" / moleculePathname;
		// args.add( moleculePath.string() );

		// Starting main application loop.
		app->start();

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
