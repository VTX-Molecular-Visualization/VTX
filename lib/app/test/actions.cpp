#include "util/app.hpp"
#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/viewpoint.hpp>
#include <app/core/action/action_system.hpp>
#include <app/core/action/base_action.hpp>
#include <app/core/action/base_action_undonable.hpp>
#include <app/core/ecs/base_entity.hpp>
#include <app/core/settings/settings_system.hpp>
#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <app/settings.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/filesystem.hpp>

namespace VTX::App::Test
{
	int COUNTER = 0;

	class TestAction final : public VTX::App::Core::Action::BaseAction
	{
	  public:
		TestAction( const int p_inc = 1 ) : _inc( p_inc ) {};

		void execute() override { COUNTER += _inc; }

	  private:
		int _inc;
	};
	class TestActionUndonable final : public VTX::App::Core::Action::BaseActionUndonable
	{
	  public:
		TestActionUndonable( const int p_inc = 1 ) : _inc( p_inc ) {};

		void execute() override { COUNTER += _inc; }
		void undo() override { COUNTER -= _inc; }

	  private:
		int _inc;
	};
} // namespace VTX::App::Test

TEST_CASE( "VTX_APP - Action - ActionManager", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	REQUIRE( !ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 0 );

	ACTION_SYSTEM().execute<Test::TestAction>();

	REQUIRE( !ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 1 );

	ACTION_SYSTEM().execute<Test::TestActionUndonable>();
	REQUIRE( ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 2 );

	ACTION_SYSTEM().execute<Test::TestActionUndonable>( 5 );
	REQUIRE( ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 7 );

	ACTION_SYSTEM().undo();
	REQUIRE( ACTION_SYSTEM().canUndo() );
	REQUIRE( ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 2 );

	ACTION_SYSTEM().undo();
	REQUIRE( !ACTION_SYSTEM().canUndo() );
	REQUIRE( ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 1 );

	ACTION_SYSTEM().execute<Test::TestActionUndonable>( 3 );
	REQUIRE( ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 4 );

	ACTION_SYSTEM().undo();
	REQUIRE( !ACTION_SYSTEM().canUndo() );
	REQUIRE( ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 1 );

	ACTION_SYSTEM().redo();
	REQUIRE( ACTION_SYSTEM().canUndo() );
	REQUIRE( !ACTION_SYSTEM().canRedo() );
	REQUIRE( Test::COUNTER == 4 );
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
	// ACTION_SYSTEM().execute<Action::Application::SaveScene>( scenePath ); // Save in an other file and check
	// existence
	// CHECK( SCENE().getItemCount() == 1 );

	ACTION_SYSTEM().execute<Action::Application::NewScene>(); // Check if scene is empty
	CHECK( SCENE().getItemCount() == 0 );

	// ACTION_SYSTEM().execute<Action::Application::OpenScene>( scenePath ); // Check if scene is well loaded
	// CHECK( SCENE().getItemCount() == 1 );
}

TEST_CASE( "VTX_APP - Action - Scene - Viewpoints", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	SCENE().getCamera().getTransform().moveFront( 5 );
	SCENE().getCamera().getTransform().localRotate( { 45, 45, 0 } );

	ACTION_SYSTEM().execute<Action::Scene::CreateViewpoint>();

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
	const App::Core::Settings::SettingsSystem settings = SETTINGS_SYSTEM();

	SETTINGS_SYSTEM().set<float>( Settings::Camera::FAR_CLIP_KEY, 33.f );

	const App::Core::Settings::SettingsSystem modifiedSettings = SETTINGS_SYSTEM();

	/*
	if ( std::filesystem::exists( Filesystem::getSettingJsonFile() ) )
		std::filesystem::remove( Filesystem::getSettingJsonFile() );

	CHECK( !std::filesystem::exists( Filesystem::getSettingJsonFile() ) );
	ACTION_SYSTEM().execute<Action::Application::SaveSettings>();
	CHECK( std::filesystem::exists( Filesystem::getSettingJsonFile() ) );
	CHECK( SETTINGS_SYSTEM() == modifiedSettings );

	ACTION_SYSTEM().execute<Action::Application::ResetSettings>();
	CHECK( SETTINGS_SYSTEM() == settings );

	ACTION_SYSTEM().execute<Action::Application::LoadSettings>();
	CHECK( SETTINGS_SYSTEM() == modifiedSettings );
	*/
};
