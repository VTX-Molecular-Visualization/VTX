#include "util/app.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/action/base_action.hpp>
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/viewpoint.hpp>
#include <app/core/ecs/base_entity.hpp>
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <app/settings/settings.hpp>
#include <app/settings/settings_manager.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/filesystem.hpp>

TEST_CASE( "VTX_APP - Action - ActionManager", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;
};

TEST_CASE( "VTX_APP - Action - Application", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;
	Test::Util::App::loadTestSystem();

	std::filesystem::create_directory( Util::Filesystem::getExecutableDir() / "data/actions" );
	// const FilePath scenePath = Util::Filesystem::getExecutableDir() / "data/actions/scene.vtx";

	CHECK( SCENE().getItemCount() == 1 );
	// ACTION().execute<Action::Application::SaveScene>( scenePath ); // Save in an other file and check
	// existence
	// CHECK( SCENE().getItemCount() == 1 );

	ACTION().execute<Action::Application::NewScene>(); // Check if scene is empty
	CHECK( SCENE().getItemCount() == 0 );

	// ACTION().execute<Action::Application::OpenScene>( scenePath ); // Check if scene is well loaded
	// CHECK( SCENE().getItemCount() == 1 );
}

TEST_CASE( "VTX_APP - Action - Scene - Viewpoints", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	SCENE().getCamera().getTransform().moveFront( 5 );
	SCENE().getCamera().getTransform().localRotate( { 45, 45, 0 } );

	ACTION().execute<Action::Scene::CreateViewpoint>();

	REQUIRE( SCENE().getItem( "Viewpoint" ) != App::Core::ECS::INVALID_ENTITY );

	const Component::Render::Viewpoint & viewpoint
		= SCENE().getComponentByName<Component::Render::Viewpoint>( "Viewpoint" );

	CHECK( viewpoint.getPosition() == SCENE().getCamera().getTransform().getPosition() );
	CHECK( viewpoint.getRotation() == SCENE().getCamera().getTransform().getRotation() );
};

TEST_CASE( "VTX_APP - Action - Application - Settings", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	// Settings
	const App::Settings::SettingsManager settings = SETTINGS();

	SETTINGS().set<float>( Settings::Camera::FAR_CLIP_KEY, 33.f );

	const App::Settings::SettingsManager modifiedSettings = SETTINGS();

	/*
	if ( std::filesystem::exists( Filesystem::getSettingJsonFile() ) )
		std::filesystem::remove( Filesystem::getSettingJsonFile() );

	CHECK( !std::filesystem::exists( Filesystem::getSettingJsonFile() ) );
	ACTION().execute<Action::Application::SaveSettings>();
	CHECK( std::filesystem::exists( Filesystem::getSettingJsonFile() ) );
	CHECK( SETTINGS() == modifiedSettings );

	ACTION().execute<Action::Application::ResetSettings>();
	CHECK( SETTINGS() == settings );

	ACTION().execute<Action::Application::LoadSettings>();
	CHECK( SETTINGS() == modifiedSettings );
	*/
};
