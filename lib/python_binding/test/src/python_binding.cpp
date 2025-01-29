#include "util/app.hpp"
#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

void runScript( const std::string & p_scriptName, const VTX::PythonBinding::Interpretor & p_interpretor )
{
	using namespace VTX;

	const FilePath	  scriptPath   = App::Filesystem::getInternalDataDir() / ( p_scriptName + ".py" );
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	p_interpretor.runCommand( ssCommandRun.str() );
};

TEST_CASE( "VTX_PYTHON_BINDING - Script test", "[python][script][print]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;
	PythonBinding::Interpretor &   interpretor = INTERPRETOR();

	REQUIRE( App::SCENE().getItemCount() == 0 );

	const FilePath systemPath
		= App::Filesystem::getInternalDataDir() / App::Test::Util::PythonFixture::MOLECULE_TEST_NAME_EXT;

	App::Action::Application::Open openAction = App::Action::Application::Open( systemPath );
	openAction.execute();

	std::stringstream ssCommandOpen = std::stringstream();
	ssCommandOpen << "openFile( path=" << systemPath << " )";

	interpretor.runCommand( ssCommandOpen.str() );
	REQUIRE( App::SCENE().getItemCount() == 2 );

	try
	{
		std::stringstream ssBadCommandOpen = std::stringstream();
		ssBadCommandOpen << "openFile( tirelipimpon=" << systemPath << " )";
		interpretor.runCommand( ssBadCommandOpen.str() );
		CHECK( false );
	}
	catch ( const CommandException & e )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	const FilePath scriptPath = App::Filesystem::getInternalDataDir() / "script_test.py";

	try
	{
		interpretor.runScript( scriptPath );
		CHECK( true );
	}
	catch ( const CommandException & e )
	{
		CHECK( false );
		VTX_ERROR( "{}", e.what() );
	}
	catch ( const std::exception & e )
	{
		CHECK( false );
		VTX_ERROR( "{}", e.what() );
	}

	const FilePath badScriptPath = App::Filesystem::getInternalDataDir() / "bad_script_test.py";

	try
	{
		interpretor.runScript( badScriptPath );
		CHECK( false );
	}
	catch ( const ScriptException & e )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	std::stringstream ssCommandRun = std::stringstream();
	ssCommandRun << "runScript(" << scriptPath << " )";

	try
	{
		interpretor.runCommand( ssCommandRun.str() );
		CHECK( true );
	}
	catch ( const ScriptException & e )
	{
		CHECK( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}
	catch ( const std::exception & e )
	{
		REQUIRE( false );
		VTX_ERROR( "bad exception catch : {}", e.what() );
	}

	return; // TODO : put these bellow back once the serialization is up and running

	const FilePath scenePath = App::Filesystem::getInternalDataDir() / "scene_test.vtx";

	if ( std::filesystem::exists( scenePath ) )
		std::filesystem::remove( scenePath );

	CHECK( !std::filesystem::exists( scenePath ) );
	std::stringstream ssCommandSaveScene = std::stringstream();
	ssCommandSaveScene << "save(" << scenePath << " )";
	interpretor.runCommand( ssCommandSaveScene.str() );
	CHECK( std::filesystem::exists( scenePath ) );

	CHECK( App::SCENE().getItemCount() > 0 );
	interpretor.runCommand( "newScene()" );
	CHECK( App::SCENE().getItemCount() == 0 );

	std::stringstream ssCommandLoadScene = std::stringstream();
	ssCommandLoadScene << "openScene(" << scenePath << " )";
	interpretor.runCommand( ssCommandLoadScene.str() );
	CHECK( App::SCENE().getItemCount() > 0 );

	interpretor.runCommand( "saveSettings()" );
	interpretor.runCommand( "resetSettings()" );
	interpretor.runCommand( "reloadSettings()" );
};

TEST_CASE( "VTX_PYTHON_BINDING - External tool benchmark", "[.][integration]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();
	interpretor.init();

	const FilePath systemPath
		= App::Filesystem::getInternalDataDir() / App::Test::Util::PythonFixture::MOLECULE_TEST_NAME_EXT;

	VTX::App::Action::Scene::LoadSystem openAction = VTX::App::Action::Scene::LoadSystem( systemPath );
	openAction.execute();

	BENCHMARK( "atom_name_access_1" ) { runScript( "atom_name_access_1", interpretor ); };
	BENCHMARK( "atom_name_access_2" ) { runScript( "atom_name_access_2", interpretor ); };
};
