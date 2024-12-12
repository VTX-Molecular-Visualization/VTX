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

TEST_CASE( "VTX_PYTHON_BINDING - VTX API Tests", "[integration]" )
{
	using namespace VTX;
	return;
	App::Test::Util::PythonFixture f;
	PythonBinding::Interpretor & interpretor = INTERPRETOR();
	interpretor.init();

	const FilePath moleculePath
		= App::Filesystem::getInternalDataDir() / App::Test::Util::PythonFixture::MOLECULE_TEST_NAME_EXT;

	App::Action::Scene::LoadSystem loadSystemAction = App::Action::Scene::LoadSystem( moleculePath );
	loadSystemAction.execute();
	const FilePath	  scriptPath   = App::Filesystem::getInternalDataDir() / "test_api.py";
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	interpretor.runCommand( ssCommandRun.str() );

	interpretor.runCommand( "countAtoms( select( mol_n='1AGA' ) )" );
	interpretor.runCommand( "countAtoms( select( mol_n={'1AGA', '1AGA', '8ODO'} ) )" );
};
