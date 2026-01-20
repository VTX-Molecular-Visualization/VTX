#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <pybind11/embed.h>
#include <python_binding/interpretor.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - Interpretor runCommand test", "[python][binding][interpretor]" )
{
	VTX::App::Test::Util::PythonFixture f;

	CHECK( f.interpretor.runCommand( "l = (1,2,3)" ).empty() );
	CHECK( f.interpretor.runCommand( "l" ) == "(1, 2, 3)" );

	// Here we test that the command is executed only once despite the execption shenanigan
	f.interpretor.runCommand( "incr = 1" );
	f.interpretor.runCommand( "incr += 1" );
	CHECK( f.interpretor.runCommand( "incr" ) == "2" );
	CHECK( f.interpretor.runCommand( "l = [1,2,3]" ).empty() );
	CHECK( f.interpretor.runCommand( "l.append(4)" ).empty() );
	CHECK( f.interpretor.runCommand( "l" ) == "[1, 2, 3, 4]" );
}

TEST_CASE( "VTX_PYTHON_BINDING - Python version", "[python][binding][version]" )
{
	VTX::App::Test::Util::PythonFixture f;

	CHECK( f.interpretor.runCommand( "import sys" ).empty() );
	std::string v = f.interpretor.runCommand( "sys.version" );

	bool rightVersion
		= v.find( CPYTHON_VERSION_MAJOR "." CPYTHON_VERSION_MINOR "." CPYTHON_VERSION_PATCH ) != std::string::npos;
	if ( not rightVersion )
	{
		VTX::VTX_INFO( "Python version used : <{}>", v );
		VTX::VTX_INFO( "Python Executable used : <{}>", f.interpretor.runCommand( "sys.executable" ) );
	}

	CHECK( rightVersion );
}
