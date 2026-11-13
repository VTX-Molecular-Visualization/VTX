#include "external_tool/action.hpp"
#include "external_tool/binding.hpp"
#include "util/fixture.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <python_binding/interpretor.hpp>
#include <python_binding/wrapper/object.hpp>
#include <string>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <utility>

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][action]" )
{
	using namespace VTX;
	PythonBinding::Test::Fixture f;

	PythonBinding::Interpretor & interpretor = f.interpretor;

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.add( VTX::PythonBinding::Binder( VTX::Test::ExternalTool::Binder() ) );
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == false );

	Test::ExternalTool::Action::ToolAction::reset();
	interpretor.runCommand( "vtx.ToolActionExecute()" );
	CHECK( Test::ExternalTool::Action::ToolAction::executed() == true );
}

TEST_CASE( "VTX_PYTHON_BINDING - Module loading", "[python][binding][module]" )
{
	using namespace VTX;

	const PythonBinding::Test::Fixture f;

	const PythonBinding::Interpretor & interpretor = f.interpretor;

	const FilePath scriptPath
		= VTX::Util::Filesystem::getExecutableDir()
		  / "data" // This one is technically a constant duplication, but it is placed at a sub-optimal location
				   // throughout VTX (i.e. App) and is not accessible. Maybe this shall be moved toward Util instead.
		  / "custom_module.py";
	REQUIRE( std::filesystem::exists( scriptPath ) == true );
	const PythonBinding::Wrapper::Module customModule = interpretor.loadModule( scriptPath );

	customModule.displayInfo();

	CHECK_NOTHROW( customModule.runFunction<std::string>( "testStr" ) );
	CHECK_NOTHROW( customModule.runFunction<std::string>( "testStr", "VTX" ) );

	CHECK_THROWS_AS( customModule.runFunction<int>( "iDontExist", "from VTX" ), PythonWrapperException );
	CHECK_THROWS_AS(
		( customModule.runFunction<int, int>(
			"testStr", 182, std::pair<std::string, float>( "false signature", 4.f )
		) ),
		PythonWrapperException
	);
	CHECK_THROWS_AS(
		( customModule.runFunction<std::pair<int, int>>( "testStr", "testStrWithParam" ) ), PythonWrapperException
	);

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
