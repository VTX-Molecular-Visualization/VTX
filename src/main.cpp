#include <io/internal/filesystem.hpp>
#include <string>
#include <ui/core/base_ui_application.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

// TODO: move to conan.
#define VTX_UI_QT

#ifdef VTX_UI_QT
#include <qt/application_qt.hpp>
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
		const FilePath logDir = VTX::Util::Filesystem::getExecutableDir();
		LOGGER().init( logDir );

#ifdef VTX_UI_QT
		auto * app = UI::QT::QT_APP();
#else
		auto * app = &APP();
#endif

		app->start( { p_argv, p_argv + p_argc } );

		// std::unique_ptr<VTX::UI::Core::BaseUIApplication> vtxApplication = UI::UIGenerator::createUI();
		// VTX::UI::Environment::get().setUIApp( vtxApplication.get() );
		// vtxApplication->init();

		// const std::vector<std::string> args( p_argv, p_argv + p_argc );
		// vtxApplication->start( std::vector( args.begin() + 1, args.end() ) );

		const FilePath molPath = IO::Internal::Filesystem::getInternalDataDir() / "md_0_1.gro";
		// vtxApplication->start( { molPath.string() } );

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
