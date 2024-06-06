#include "vtx_tool_include.hpp"
#include <io/internal/filesystem.hpp>
#include <string>
#include <ui/core/base_ui_application.hpp>
#include <ui/environment.hpp>
#include <ui/ui_generator.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement			 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

// #ifdef VTX_PRODUCTION
//  Hide console.
// #pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
// #endif

int main( int p_argc, char * p_argv[] )
{
	using namespace VTX;

	//::ShowWindow( ::GetConsoleWindow(), SW_HIDE );

	try
	{
		const FilePath logDir = VTX::Util::Filesystem::getExecutableDir();
		Util::Logger::get().init( logDir );

		std::unique_ptr<VTX::UI::Core::BaseUIApplication> vtxApplication = UI::UIGenerator::createUI();
		VTX::UI::Environment::get().setUIApp( vtxApplication.get() );
		vtxApplication->init();

		// const std::vector<std::string> args( p_argv, p_argv + p_argc );
		// vtxApplication->start( std::vector( args.begin() + 1, args.end() ) );

		const FilePath molPath = IO::Internal::Filesystem::getInternalDataDir() / "1AGA.mmtf";
		vtxApplication->start( { molPath.string() } );

		VTX::Util::Logger::get().stop();
		return vtxApplication->getReturnCode();
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		VTX_ERROR( "{}", error );
		VTX::Util::Logger::get().stop();
		return EXIT_FAILURE;
	}
}
