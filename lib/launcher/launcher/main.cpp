#include "vtx_tool.hpp"
#include <string>
#include <ui/core/base_ui_application.hpp>
#include <ui/environment.hpp>
#include <ui/ui_generator.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

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
		VTX::UI::Core::BaseUIApplication * const vtxApplication = UI::UIGenerator::createUI();
		VTX::UI::Environment::get().setUIApp( vtxApplication );
		vtxApplication->init();

		const std::vector<std::string> args( p_argv, p_argv + p_argc );
		vtxApplication->start( std::vector( args.begin() + 1, args.end() ) );

		return vtxApplication->getReturnCode();
	}
	catch ( const std::exception & p_e )
	{
		std::string error = p_e.what();
		VTX_ERROR( p_e.what() );
#ifdef VTX_PRODUCTION
		UI::Dialog::unhandledException();
#else
		return EXIT_FAILURE;
#endif
	}
}
