#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

void runScript( const std::string & p_scriptName, const VTX::PythonBinding::Interpretor & p_interpretor )
{
	using namespace VTX;

	const FilePath	  scriptPath   = IO::Internal::Filesystem::getInternalDataDir() / ( p_scriptName + ".py" );
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	p_interpretor.runCommand( ssCommandRun.str() );
};

TEST_CASE( "VTX_PYTHON_BINDING - Interpretor test", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	std::unique_ptr<PythonBinding::Interpretor> interpretorPtr = App::Test::Util::App::createInterpretor();
	PythonBinding::Interpretor &				interpretor	   = *interpretorPtr;
	interpretor.init();

	REQUIRE( App::SCENE().getItemCount() == 0 );

	const FilePath moleculePath
		= IO::Internal::Filesystem::getInternalDataDir() / App::Test::Util::App::MOLECULE_TEST_NAME_EXT;

	App::Internal::Action::ECS::Open openAction = App::Internal::Action::ECS::Open( moleculePath );
	openAction.execute();

	REQUIRE( App::SCENE().getItemCount() == 1 );

	std::stringstream ssCommandOpen = std::stringstream();
	ssCommandOpen << "openFile( path=" << moleculePath << " )";

	interpretor.runCommand( ssCommandOpen.str() );
	REQUIRE( App::SCENE().getItemCount() == 2 );

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

TEST_CASE( "VTX_PYTHON_BINDING - External tool benchmark", "[.][integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	std::unique_ptr<PythonBinding::Interpretor> interpretor = App::Test::Util::App::createInterpretor();
	interpretor->init();

	const FilePath moleculePath
		= IO::Internal::Filesystem::getInternalDataDir() / App::Test::Util::App::MOLECULE_TEST_NAME_EXT;

	App::Internal::Action::ECS::Open openAction = App::Internal::Action::ECS::Open( moleculePath );
	openAction.execute();

	BENCHMARK( "atom_name_access_1" ) { runScript( "atom_name_access_1", *interpretor ); };
	BENCHMARK( "atom_name_access_2" ) { runScript( "atom_name_access_2", *interpretor ); };
};
