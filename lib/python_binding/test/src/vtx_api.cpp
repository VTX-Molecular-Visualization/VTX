#include "util/app.hpp"
#include <app/action/scene.hpp>
#include <app/filesystem.hpp>
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

	App::Test::Util::App::initApp();

	PythonBinding::Interpretor & interpretor = INTERPRETOR();
	interpretor.init();

	const FilePath moleculePath = App::Filesystem::getInternalDataDir() / App::Test::Util::App::MOLECULE_TEST_NAME_EXT;

	App::Action::Scene::LoadMolecule loadMoleculeAction = App::Action::Scene::LoadMolecule( moleculePath );
	loadMoleculeAction.execute();

	const FilePath	  scriptPath   = App::Filesystem::getInternalDataDir() / "test_api.py";
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	interpretor.runCommand( ssCommandRun.str() );

	interpretor.runCommand( "countAtoms( select( mol_n='1AGA' ) )" );
	interpretor.runCommand( "countAtoms( select( mol_n={'1AGA', '1AGA', '8ODO'} ) )" );
};
