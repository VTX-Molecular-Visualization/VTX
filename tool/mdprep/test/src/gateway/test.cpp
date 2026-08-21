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
#include <latch>
#include <string>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>
//
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <util/thread/base_thread.hpp>
//
#include <tool/mdprep/backends/gromacs/util.hpp>
#include <tool/mdprep/gateway/engine_job_manager.hpp>
#include <tool/mdprep/gateway/form_data.hpp>
#include <tool/mdprep/gateway/shared.hpp>
#include <util/filesystem.hpp>
//
#include <tool/mdprep/gateway/backend_gromacs.hpp>

namespace fs = std::filesystem;
using VTX::FilePath;

TEST_CASE( "VTX_TOOL_MdPrep - integration", "[integration]" )
{
	VTX::App::Fixture f;

	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>( VTX::App::SESSION().getDataDir() / "2qwo.nolig.pdb" );
	VTX::Tool::Mdprep::Gateway::Gromacs::MdSettings settings;
	VTX::Tool::Mdprep::Gateway::Gromacs::JobManager jobManager( settings );
	VTX::Tool::Mdprep::Gateway::MdParameters		mdParams;
	mdParams.system.forcefieldBio = "gromos54a7";

	VTX::Util::Thread::ThreadData thrData;
	jobManager.startPreparation( mdParams );
	/*


	const FilePath path = VTX::App::SESSION().getLogsDir();
	fs::create_directory( path );
	VTX::Util::Logger::init( path );

	VTX::App::Fixture app;
	VTX::App::SCENE().reset();

	const char *						itemName		 = "2QWO";
	const FilePath					systemPath		 = VTX::App::SESSION().getDataDir() / "2qwo.nolig.pdb";

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
