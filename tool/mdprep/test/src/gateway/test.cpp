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
#include <tool/mdprep/gateway/engine_job_manager.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - test", "[test]" )
{
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
