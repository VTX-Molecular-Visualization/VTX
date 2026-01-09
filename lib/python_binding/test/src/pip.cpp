
#include "util/app.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <python_binding/interpretor.hpp>
#include <util/filesystem.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - Numpy module installation test", "[python][binding][pip]" )
{
#ifdef _DEBUG
	/**
	 * @brief This test is bound to fail in debug as numpy will look for files whom's name is declined in debug mode.
	 * Consequently, the lookout will fail and so the test.
	 * If "unicode" package is not found on windows, maybe the package fetched by conan did get the whole thing. The
	 * DLLs folder is mandatory for it to work as it contains binaries needed for pip to work and it already happened
	 * that this folder wasn't in the package.
	 */
	INFO( "pip can't run in debug mode." );
	return;
#endif // DEBUG
	using namespace VTX;
	App::Test::Util::PythonFixture f;

	PythonBinding::Interpretor & interpretor = f.interpretor;

	VTX::FilePath pythonHome = Util::Filesystem::getExecutableDir() / "external" / "python";

	interpretor.runCommand( "import sys" );
	interpretor.runCommand( "print( sys.path)" );
	interpretor.runCommand( "import subprocess" );

#ifdef _WIN32
	std::string pythonPath	 = ( pythonHome / "python.exe" ).string();
	size_t		backslashPos = pythonPath.find( "\\" );
	while ( backslashPos != std::string::npos )
	{
		pythonPath.replace( backslashPos, 1, "\\\\" );
		backslashPos = pythonPath.find( "\\", backslashPos + 2 );
	}
#else
	std::string pythonPath = ( pythonHome / "bin" / "python" ).string();
#endif // _WIN32

	interpretor.runCommand( fmt::format( "subprocess.run('{} -m pip install numpy', shell=True)", pythonPath ) );
	// interpretor.runCommand( "import pip" );
	// interpretor.runCommand( "pip.main(['install', 'numpy'])" );
	interpretor.runCommand( "import numpy" );
	std::string npArray = interpretor.runCommand( "numpy.array([1,2,3])" );
	CHECK( npArray == "array([1, 2, 3])" );
}
