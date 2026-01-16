
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

	std::string pip_install_dir = ( pythonHome / "Lib" / "site-packages" ).string();
#else
	std::string pythonPath		= ( pythonHome / "bin" / "python" ).string();
	std::string pip_install_dir = ( pythonHome / "lib" / "python3.9" / "site-packages" ).string();
#endif // _WIN32
	CHECK_NOFAIL( std::filesystem::exists( pip_install_dir ) );

	interpretor.runCommand( fmt::format( "subprocess.run('{} -m pip install numpy', shell=True)", pythonPath ) );
	bool has_numpy = false;
	std::ranges::for_each(
		std::filesystem::directory_iterator( pip_install_dir ),
		[ has_numpy = &has_numpy ]( const std::filesystem::path & dir_entry )
		{
			if ( dir_entry.string().find( "numpy" ) != std::string::npos )
				*has_numpy = true;
		}
	);

	CHECK( has_numpy );
	// interpretor.runCommand( "import pip" );
	// interpretor.runCommand( "pip.main(['install', 'numpy'])" );
	interpretor.runCommand( "import numpy" );
	std::string npArray = interpretor.runCommand( "numpy.array([1,2,3])" );
	CHECK( npArray == "array([1, 2, 3])" );
}
