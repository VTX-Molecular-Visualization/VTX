#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/binding/vtx_app_binder.hpp>
#include <python_binding/interpretor.hpp>
#include <sstream>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_PYTHON_BINDING - VTX API Tests", "[.][integration]" )
{
	using namespace VTX;

	App::Test::Util::App::initApp();

	std::unique_ptr<PythonBinding::Interpretor> interpretor = App::Test::Util::App::createInterpretor();
	interpretor->init();

	App::Application::Scene & scene = App::VTXApp::get().getScene();

	const FilePath moleculePath
		= IO::Internal::Filesystem::getInternalDataDir() / App::Test::Util::App::MOLECULE_TEST_NAME_EXT;

	App::Internal::Action::ECS::Open openAction = App::Internal::Action::ECS::Open( moleculePath );
	openAction.execute();

	const FilePath	  scriptPath   = IO::Internal::Filesystem::getInternalDataDir() / "test_api.py";
	std::stringstream ssCommandRun = std::stringstream();

	ssCommandRun << "runScript(" << scriptPath << " )";
	interpretor->runCommand( ssCommandRun.str() );

	interpretor->runCommand( "countAtoms( select( mol_n='1AGA' ) )" );
	interpretor->runCommand( "countAtoms( select( mol_n={'1AGA', '1AGA', '8ODO'} ) )" );
};