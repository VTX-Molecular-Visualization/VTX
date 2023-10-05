#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
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

	std::stringstream ssCommandOpen = std::stringstream();
	ssCommandOpen << "openFile( path=" << moleculePath << " )";

	interpretor.runCommand( ssCommandOpen.str() );
	REQUIRE( scene.getItemCount() == 2 );

	try
	{
		std::stringstream ssBadCommandOpen = std::stringstream();
		ssBadCommandOpen << "openFile( tirelipimpon=" << moleculePath << " )";
		interpretor.runCommand( ssBadCommandOpen.str() );
	}
	catch ( const CommandException & e )
	{
		VTX_INFO( "CommandException : {}", e.what() );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	const FilePath scriptPath = IO::Internal::Filesystem::getInternalDataDir() / "script_test.py";

	try
	{
		interpretor.runScript( scriptPath );
	}
	catch ( const CommandException & e )
	{
		VTX_INFO( "{}", e.what() );
	}

	const FilePath badScriptPath = IO::Internal::Filesystem::getInternalDataDir() / "bad_script_test.py";

	try
	{
		interpretor.runScript( badScriptPath );
	}
	catch ( const ScriptException & e )
	{
		VTX_INFO( "{}", e.what() );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	std::stringstream ssCommandRun = std::stringstream();
	ssCommandRun << "runScript(" << scriptPath << " )";

	interpretor.runCommand( ssCommandRun.str() );
};
