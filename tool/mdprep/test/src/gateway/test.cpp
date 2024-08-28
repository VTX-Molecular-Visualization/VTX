#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>
//
#include <tool/mdprep/gateway/engine_job_manager.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - test", "[test]" )
{
	const VTX::FilePath path = VTX::Util::Filesystem::getExecutableDir() / "logs";
	std::filesystem::create_directory( path );
	VTX::Util::Logger::get().init( path );

	VTX::App::VTXApp::get().start( {} );

	VTX::App::SCENE().reset();

	const VTX::FilePath moleculePath = VTX::IO::Internal::Filesystem::getInternalDataDir() / "2qwo.nolig.pdb";

	VTX::App::Action::Scene::LoadMolecule loadMoleculeAction = VTX::App::Action::Scene::LoadMolecule( moleculePath );
	loadMoleculeAction.execute();

	auto moleculeList = VTX::App::SCENE().getAllSceneItemsOfType<VTX::App::Component::Chemistry::Molecule>();
}
