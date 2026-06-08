#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
/*
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/system.hpp>
*/
#include <app/fixture.hpp>
#include <app/vtx_app.hpp>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>
//
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>
#include <tool/mdprep/gateway/backend_gromacs.hpp>
#include <tool/mdprep/gateway/engine_job_manager.hpp>
#include <util/filesystem.hpp>

namespace fs = std::filesystem;

TEST_CASE( "VTX_TOOL_MdPrep - integration", "[integration]" )
{
	VTX::App::Fixture f;
	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
		VTX::Util::Filesystem::getExecutableDir() / "data" / "2qwo.nolig.pdb"
	);

	VTX::Tool::Mdprep::Gateway::Gromacs::JobManager jobManager( VTX::Tool::Mdprep::Gateway::Gromacs::MdSettings() );
	/*
	const VTX::FilePath path = VTX::Util::Filesystem::getExecutableDir() / "logs";
	std::filesystem::create_directory( path );
	VTX::Util::Logger::init( path );

	VTX::App::Fixture app;
	VTX::App::SCENE().reset();

	const char *						itemName		 = "2QWO";
	const VTX::FilePath					systemPath		 = VTX::App::Filesystem::getDataDir() / "2qwo.nolig.pdb";
	VTX::App::Action::Scene::LoadSystem loadSystemAction = VTX::App::Action::Scene::LoadSystem( systemPath );
	loadSystemAction.execute();

	VTX::App::Core::ECS::BaseEntity molEntity = VTX::App::SCENE().getItem( itemName );
	REQUIRE( VTX::App::ECS_REGISTRY().isValid( molEntity ) );
	{
		VTX::App::Component::Chemistry::System & chemSystem
			= VTX::App::ECS_REGISTRY().getComponent<VTX::App::Component::Chemistry::System>( molEntity );
	}
	return;
	*/
}
