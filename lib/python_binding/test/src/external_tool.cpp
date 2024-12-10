#include "external_tool/binding.hpp"
#include "util/app.hpp"
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

TEST_CASE( "VTX_PYTHON_BINDING - External tool test", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::PythonFixture f;
	return;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	interpretor.addBinder<VTX::Test::ExternalTool::Binder>();
	interpretor.init();

	interpretor.runCommand( "new_command()" );

	const FilePath scriptPath = App::Filesystem::getInternalDataDir() / "custom_module.py";
	REQUIRE( std::filesystem::exists( scriptPath ) == true );
	const PythonBinding::Wrapper::Module customModule = interpretor.loadModule( scriptPath );

	customModule.displayInfo();

	VTX_INFO( "<{}> at {}", customModule.runFunction<std::string>( "testStr" ), src_info() );
	VTX_INFO( "<{}> at {}", customModule.runFunction<std::string>( "testStr", "VTX" ), src_info() );

	try
	{
		customModule.runFunction<int>( "iDontExist", "from VTX" );
	}
	catch ( const PythonWrapperException & exception )
	{
		VTX_INFO( "Exception managed : <{}> at {}", exception.what(), src_info() );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
	}

	try
	{
		customModule.runFunction<int, int>( "testStr", 182, std::pair<std::string, float>( "false signature", 4.f ) );
	}
	catch ( const PythonWrapperException & exception )
	{
		VTX_INFO( "Exception managed : <{}> at {}", exception.what(), src_info() );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
	}

	try
	{
		customModule.runFunction<std::pair<int, int>>( "testStr", "testStrWithParam" );
	}
	catch ( const PythonWrapperException & exception )
	{
		VTX_INFO( "Exception managed : <{}> at {}", exception.what(), src_info() );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "<{}> at {}", e.what(), src_info() );
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
	}
	VTX_INFO( "Reached the end of the UT at {}", src_info() );
};
