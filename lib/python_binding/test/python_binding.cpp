#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/interpretor.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_APP - Python binding - Load molecule test", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();
	PythonBinding::Interpretor interpretor = PythonBinding::Interpretor();

	App::Application::Scene & scene = App::VTXApp::get().getScene();

	REQUIRE( scene.getItemCount() == 0 );

	const FilePath moleculePath
		= IO::Internal::Filesystem::getInternalDataDir() / App::Test::Util::App::MOLECULE_TEST_NAME_EXT;

	App::Internal::Action::ECS::Open openAction = App::Internal::Action::ECS::Open( moleculePath );
	openAction.execute();

	REQUIRE( scene.getItemCount() == 1 );

	const std::string commandStr = "openFile " + moleculePath.string();

	try
	{
		// interpretor.runCommand( commandStr );
		interpretor.runTest();
	}
	catch ( const std::exception e )
	{
		VTX_ERROR( "Exception : {}", e.what() );
	}

	REQUIRE( scene.getItemCount() == 2 );
};
