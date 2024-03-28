#include "include_tools.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <ui/debug/print_inputs.hpp>
#include <ui/environment.hpp>
#include <ui/ui_generator.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement			 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

TEST_CASE( "VTX_UI - Test", "[integration]" )
{
	using namespace VTX;

	const FilePath logDir = std::filesystem::current_path();
	Util::Logger::get().init( logDir );

	try
	{
		std::unique_ptr<VTX::UI::Core::BaseUIApplication> vtxApplicationPtr = UI::UIGenerator::createUI();
		VTX::UI::Environment::get().setUIApp( vtxApplicationPtr.get() );
		vtxApplicationPtr->init();

		const FilePath molPath = IO::Internal::Filesystem::getInternalDataDir() / "1AGA.mmtf";

		// Uncomment to print inputs
		// UI::Debug::PrintInputs();

		vtxApplicationPtr->start( { molPath.string() } );

		VTX_INFO( "Return code : {}", vtxApplicationPtr->getReturnCode() );
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		VTX_ERROR( "{}", error );
	}

	VTX::Util::Logger::get().stop();
}

// TEST_CASE( "VTX_UI - Benchmark", "[.][perfs]" ) {}
