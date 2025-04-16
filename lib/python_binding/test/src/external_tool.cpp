#include "external_tool/action.hpp"
#include "external_tool/binding.hpp"
#include "util/app.hpp"

// #include <app/filesystem.hpp>
// #include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/object.hpp>
#include <source_location>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

std::string src_info( const std::source_location location = std::source_location::current() )
{
	return std::string( "[" ) + location.file_name() + '(' + std::to_string( location.line() ) + ':'
		   + std::to_string( location.column() ) + " '" + location.function_name() + "']";
}

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][action]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.addBinder<VTX::Test::ExternalTool::Binder>();
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == false );

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.runCommand( "ToolActionExecute()" );
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == true );
}

TEST_CASE( "VTX_PYTHON_BINDING - Module loading", "[python][binding]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	const FilePath scriptPath
		= VTX::Util::Filesystem::getExecutableDir()
		  / "data" // This one is technically a constant duplication, but it is placed at a sub-optimal location
				   // throughout VTX (i.e. App) and is not accessible. Maybe this shall be moved toward Util instead.
		  / "custom_module.py";
	REQUIRE( std::filesystem::exists( scriptPath ) == true );
	const PythonBinding::Wrapper::Module customModule = interpretor.loadModule( scriptPath );

	customModule.displayInfo();

	try
	{
		customModule.runFunction<std::string>( "testStr" );
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<std::string>( "testStr", "VTX" );
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<int>( "iDontExist", "from VTX" );
		CHECK( false );
	}
	catch ( const PythonWrapperException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<int, int>( "testStr", 182, std::pair<std::string, float>( "false signature", 4.f ) );
		CHECK( false );
	}
	catch ( const PythonWrapperException & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		customModule.runFunction<std::pair<int, int>>( "testStr", "testStrWithParam" );
		CHECK( false );
	}
	catch ( const pybind11::cast_error & )
	{
		CHECK( true );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}

	try
	{
		PythonBinding::Wrapper::Object pythonObj
			= customModule.runFunction<PythonBinding::Wrapper::Object>( "generatePythonTestClass" );

		pythonObj.call( "print" );

		int value = pythonObj.call<int>( "get" );
		CHECK( value == 10 );

		pythonObj.call( "set", 5 );
		CHECK( pythonObj.call<int>( "get" ) == 5 );

		value = pythonObj.call<int, int>( "add", 7 );
		CHECK( value == 12 );
		CHECK( pythonObj.call<int>( "get" ) == 12 );

		CHECK( pythonObj.getMember<int>( "value" ) == 12 );

		pythonObj.setMember( "value", 33 );
		CHECK( pythonObj.getMember<int>( "value" ) == 33 );
	}
	catch ( PythonWrapperException & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
		CHECK( false );
	}
};

TEST_CASE( "VTX_PYTHON_BINDING - Script execution ", "[python][binding][script]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	const FilePath internalDataDir = Util::Filesystem::getExecutableDir() / "data";
	const FilePath scriptPath	   = internalDataDir / "script_test.py";


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

	const FilePath badScriptPath = internalDataDir / "bad_script_test.py";

	try
	{
		interpretor.runScript( badScriptPath );
		CHECK( false );
	}
	catch ( const ScriptException & )
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
}
