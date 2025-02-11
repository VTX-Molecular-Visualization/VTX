#include "util/app.hpp"
#include <app/action/scene.hpp>
#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

// TODO : So far we test whether the commands crash or not. Once the actual command effect is plugged back, we should
// check their result as well.
TEST_CASE( "VTX_PYTHON_BINDING - VTX API Tests", "[python][integration][api]" )
{
	using namespace VTX;
	App::Test::Util::PythonFixture f;
	PythonBinding::Interpretor &   interpretor = INTERPRETOR();
	interpretor.init();

	const FilePath systemPath
		= App::Filesystem::getInternalDataDir() / App::Test::Util::PythonFixture::MOLECULE_TEST_NAME_EXT;

	App::Action::Scene::LoadSystem loadSystemAction = App::Action::Scene::LoadSystem( systemPath );
	loadSystemAction.execute();
	const FilePath	  scriptPath   = App::Filesystem::getInternalDataDir() / "test_api.py";
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	try
	{
		interpretor.runCommand( ssCommandRun.str() );
		CHECK( true );
	}
	catch ( std::exception & e )
	{
		VTX_ERROR( "Exception catched : <{}>", e.what() );
		CHECK( false );
	}

	try
	{
		interpretor.runCommand( "countAtoms( select( mol_n='1AGA' ) )" );
		CHECK( true );
	}
	catch ( std::exception & e )
	{
		VTX_ERROR( "Exception catched : <{}>", e.what() );
		CHECK( false );
	}

	try
	{
		interpretor.runCommand( "countAtoms( select( mol_n={'1AGA', '1AGA', '8ODO'} ) )" );
		CHECK( true );
	}
	catch ( std::exception & e )
	{
		VTX_ERROR( "Exception catched : <{}>", e.what() );
		CHECK( false );
	}
};
