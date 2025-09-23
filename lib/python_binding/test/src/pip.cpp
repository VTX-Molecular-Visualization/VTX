
#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <python_binding/interpretor.hpp>
#include <util/filesystem.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - Action binding test", "[python][binding][pip]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = INTERPRETOR();

	VTX::FilePath pythonHome = Util::Filesystem::getExecutableDir() / "external" / "python";

	interpretor.runCommand( "import sys" );
	interpretor.runCommand( "print( sys.path)" );
	interpretor.runCommand( "import subprocess" );
#ifdef DEBUG
	std::string pythonPath = ( pythonHome / "python_d.exe" ).string();
#else
	std::string pythonPath = ( pythonHome / "python.exe" ).string();

#endif // DEBUG
	size_t backslashPos = pythonPath.find( "\\" );
	while ( backslashPos != std::string::npos )
	{
		pythonPath.replace( backslashPos, 1, "\\\\" );
		backslashPos = pythonPath.find( "\\", backslashPos + 2 );
	}

	interpretor.runCommand( fmt::format( "subprocess.run('{} -m pip install numpy')", pythonPath ) );
	// interpretor.runCommand( "import pip" );
	// interpretor.runCommand( "pip.main(['install', 'numpy'])" );
	interpretor.runCommand( "import numpy" );
}
