
#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/interpretor.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][pip]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	interpretor.runCommand( "import pip" );
	interpretor.runCommand( "pip.main(['install', 'numpy'])" );
	interpretor.runCommand( "import numpy" );
}
