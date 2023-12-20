#include "util/app.hpp"
#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/application/settings.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/viewpoint.hpp>
#include <app/core/action/base_action.hpp>
#include <app/internal/application/settings.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <io/internal/filesystem.hpp>
#include <util/filesystem.hpp>

template<typename Action, typename... Args>
void launchAction( const Args &... p_args )
{
	Action action = Action( p_args... );
	action.execute();
}

TEST_CASE( "VTX_APP - Action - Application", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Test::Util::App::loadTestMolecule();

	std::filesystem::create_directory( Util::Filesystem::getExecutableDir() / "data/actions" );
	const FilePath scenePath = Util::Filesystem::getExecutableDir() / "data/actions/scene.vtx";

	CHECK( SCENE().getItemCount() == 1 );
	launchAction<Action::Application::SaveScene>( scenePath ); // Save in an other file and check existence
	CHECK( SCENE().getItemCount() == 1 );

	launchAction<Action::Application::NewScene>(); // Check if scene is empty
	CHECK( SCENE().getItemCount() == 0 );

	launchAction<Action::Application::OpenScene>( scenePath ); // Check if scene is well loaded
	CHECK( SCENE().getItemCount() == 1 );
}

TEST_CASE( "VTX_APP - Action - Scene - Viewpoints", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();

	SCENE().getCamera().moveFront( 5 );
	SCENE().getCamera().rotate( { 45, 45, 0 } );

	launchAction<Action::Scene::CreateViewpoint>();

	REQUIRE( SCENE().getItem( "Viewpoint" ) != Core::ECS::INVALID_ENTITY );

	const Component::Render::Viewpoint & viewpoint
		= SCENE().getComponentByName<Component::Render::Viewpoint>( "Viewpoint" );

	CHECK( viewpoint.getPosition() == SCENE().getCamera().getPosition() );
	CHECK( viewpoint.getRotation() == SCENE().getCamera().getRotation() );
};

TEST_CASE( "VTX_APP - Action - Application - Settings", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();

	// Settings
	const Application::Settings settings = SETTINGS();

	SETTINGS().set<float>( Internal::Application::Settings::Camera::FAR_CLIP_KEY, 33.f );

	const Application::Settings modifiedSettings = SETTINGS();

	if ( std::filesystem::exists( IO::Internal::Filesystem::getSettingJsonFile() ) )
		std::filesystem::remove( IO::Internal::Filesystem::getSettingJsonFile() );

	CHECK( !std::filesystem::exists( IO::Internal::Filesystem::getSettingJsonFile() ) );
	launchAction<Action::Application::SaveSettings>();
	CHECK( std::filesystem::exists( IO::Internal::Filesystem::getSettingJsonFile() ) );
	CHECK( SETTINGS() == modifiedSettings );

	launchAction<Action::Application::ResetSettings>();
	CHECK( SETTINGS() == settings );

	launchAction<Action::Application::LoadSettings>();
	CHECK( SETTINGS() == modifiedSettings );
};
