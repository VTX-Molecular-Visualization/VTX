#include "external_tool/binding.hpp"
#include "util/app.hpp"
#include <app/filesystem.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/object.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - External tool test", "[integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	PythonBinding::Interpretor & interpretor = PythonBinding::INTERPRETOR();

	interpretor.addBinder<VTX::Test::ExternalTool::Binder>();
	interpretor.init();

	interpretor.runCommand( "new_command()" );

	const FilePath						 scriptPath	  = App::Filesystem::getInternalDataDir() / "custom_module.py";
	const PythonBinding::Wrapper::Module customModule = interpretor.loadModule( scriptPath );

	customModule.displayInfo();

	VTX_INFO( "{}", customModule.runFunction<std::string>( "testStr" ) );
	VTX_INFO( "{}", customModule.runFunction<std::string>( "testStr", "VTX" ) );

	try
	{
		customModule.runFunction<int>( "iDontExist", "from VTX" );
	}
	catch ( const PythonWrapperException exception )
	{
		VTX_INFO( "Exception managed : {}", exception.what() );
	}
	catch ( const std::exception e )
	{
		VTX_ERROR( "{}", e.what() );
	}

	try
	{
		customModule.runFunction<int, int>( "testStr", 182, std::pair<std::string, float>( "false signature", 4.f ) );
	}
	catch ( const PythonWrapperException exception )
	{
		VTX_INFO( "Exception managed : {}", exception.what() );
	}
	catch ( const std::exception e )
	{
		VTX_ERROR( "{}", e.what() );
	}

	try
	{
		customModule.runFunction<std::pair<int, int>>( "testStr", "testStrWithParam" );
	}
	catch ( const PythonWrapperException exception )
	{
		VTX_INFO( "Exception managed : {}", exception.what() );
	}
	catch ( const std::exception e )
	{
		VTX_ERROR( "{}", e.what() );
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
	catch ( PythonWrapperException e )
	{
		VTX_ERROR( "{}", e.what() );
	}
};
