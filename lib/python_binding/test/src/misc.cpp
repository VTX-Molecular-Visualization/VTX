#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <pybind11/embed.h>
#include <python_binding/interpretor.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - Interpretor runCommand test", "[python][binding][interpretor]" )
{
	VTX::App::Test::Util::PythonFixture f;

	CHECK( VTX::INTERPRETOR().runCommand( "l = (1,2,3)" ).empty() );
	CHECK( VTX::INTERPRETOR().runCommand( "l" ) == "(1, 2, 3)" );

	// Here we test that the command is executed only once despite the execption shenanigan
	VTX::INTERPRETOR().runCommand( "incr = 1" );
	VTX::INTERPRETOR().runCommand( "incr += 1" );
	CHECK( VTX::INTERPRETOR().runCommand( "incr" ) == "2" );
	CHECK( VTX::INTERPRETOR().runCommand( "l = [1,2,3]" ).empty() );
	CHECK( VTX::INTERPRETOR().runCommand( "l.append(4)" ).empty() );
	CHECK( VTX::INTERPRETOR().runCommand( "l" ) == "[1, 2, 3, 4]" );
}
