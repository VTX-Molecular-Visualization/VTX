#include "external_tool/action.hpp"
#include "external_tool/binding.hpp"
#include "util/app.hpp"
#ifdef JEVEUPAS

#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/object.hpp>
#include <source_location>
#include <util/logger.hpp>

std::string src_info( const std::source_location location = std::source_location::current() )
{
	return std::string( "[" ) + location.file_name() + '(' + std::to_string( location.line() ) + ':'
		   + std::to_string( location.column() ) + " '" + location.function_name() + "']";
}

TEST_CASE( "VTX_PYTHON_BINDING - Command binding test", "[python][binding]" )
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

	const FilePath scriptPath = App::Filesystem::getInternalDataDir() / "custom_module.py";
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
#endif // JEVEUPAS
