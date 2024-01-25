#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <ui/environment.hpp>
#include <ui/qt/tool/render_window.hpp>
#include <ui/qt/tool/ui_features/quit_tool.hpp>
#include <ui/ui_generator.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_UI - Test", "[integration]" )
{
	using namespace VTX;

	const FilePath logDir = std::filesystem::current_path();
	Util::Logger::get().init( logDir );

	try
	{
		VTX::UI::Core::BaseUIApplication * const vtxApplication = UI::UIGenerator::createUI();
		VTX::UI::Environment::get().setUIApp( vtxApplication );
		vtxApplication->init();

		const FilePath molPath = IO::Internal::Filesystem::getInternalDataDir() / "1AGA.mmtf";

		vtxApplication->start( { molPath.string() } );

		VTX_INFO( "Return code : {}", vtxApplication->getReturnCode() );
	}
	catch ( const std::exception & p_e )
	{
		const std::string error = p_e.what();
		VTX_ERROR( "{}", error );
	}
}

// TEST_CASE( "VTX_UI - Benchmark", "[.][perfs]" ) {}
