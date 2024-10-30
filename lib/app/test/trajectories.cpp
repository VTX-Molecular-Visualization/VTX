#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/chemistry/trajectory.hpp>
#include <app/core/player/base_player.hpp>
#include <app/core/player/loop.hpp>
#include <app/core/player/once.hpp>
#include <app/core/player/ping_pong.hpp>
#include <app/core/player/players.hpp>
#include <app/core/player/revert_loop.hpp>
#include <app/core/player/revert_once.hpp>
#include <app/core/player/stop.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Trajectory", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;
	Test::Util::App::loadTestTrajectorySystem();

	App::Core::ECS::BaseEntity molEntity = SCENE().getItem( App::Test::Util::App::MOLECULE_TRAJECTORY_TEST_NAME );
	App::Component::Chemistry::System & systemComponent
		= ECS_REGISTRY().getComponent<App::Component::Chemistry::System>( molEntity );

	App::Component::Chemistry::Trajectory & trajectoryComponent
		= ECS_REGISTRY().getComponent<App::Component::Chemistry::Trajectory>( molEntity );

	const size_t frameCount1NIM = 25;

	App::Core::Player::BasePlayer * playmode = nullptr;

	SECTION( "Stop playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::Stop>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( !trajectoryComponent.getPlayer().isPlaying() );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		SCENE().update( 40.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
	}

	SECTION( "Once playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::Once>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 1 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 21 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 24 );
		REQUIRE( trajectoryComponent.getPlayer().isPlaying() == false );
	}

	SECTION( "Revert once playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::RevertOnce>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 24 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 23 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 3 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		REQUIRE( trajectoryComponent.getPlayer().isPlaying() == false );
	}

	SECTION( "Loop playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::Loop>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 1 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 21 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 16 );
		REQUIRE( trajectoryComponent.getPlayer().isPlaying() == true );
	}

	SECTION( "Revert loop playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::RevertLoop>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 24 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 23 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 3 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 8 );
		REQUIRE( trajectoryComponent.getPlayer().isPlaying() == true );
	}

	SECTION( "Ping Pong playmode" )
	{
		playmode = Util::Singleton<App::Core::Player::Players>::get().getOrCreate<App::Core::Player::PingPong>();

		trajectoryComponent.setPlayer( playmode );
		trajectoryComponent.getPlayer().reset();
		trajectoryComponent.getPlayer().setFPS( 1 );

		REQUIRE( trajectoryComponent.getCurrentFrame() == 0 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 1 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 21 );
		SCENE().update( 20.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 7 );
		SCENE().update( 8.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 1 );
		SCENE().update( 50.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 3 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 4 );
		SCENE().update( 72.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 20 );
		SCENE().update( 1.f );
		REQUIRE( trajectoryComponent.getCurrentFrame() == 19 );
		REQUIRE( trajectoryComponent.getPlayer().isPlaying() == true );
	}
};
