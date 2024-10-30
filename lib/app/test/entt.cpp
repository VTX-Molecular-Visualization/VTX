#include "util/app.hpp"
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/chemistry/system.hpp>
#include <app/component/render/proxy_system.hpp>
#include <app/entity/system.hpp>
#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <renderer/proxy/system.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

TEST_CASE( "VTX_APP - Views", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;
	Test::Util::App::loadTestSystem();

	App::Core::ECS::View view1Element = SCENE().getAllSceneItems();
	REQUIRE( view1Element.size() == 1 );

	const App::Component::Scene::SceneItemComponent & sceneItemComponent
		= view1Element.getComponent<App::Component::Scene::SceneItemComponent>( view1Element.front() );
	REQUIRE( sceneItemComponent.getName() == App::Test::Util::App::MOLECULE_TEST_NAME );

	App::Core::ECS::View allSystems = SCENE().getAllSceneItemsOfType<Component::Chemistry::System>();
	REQUIRE( allSystems.size() == 1 );

	App::Core::ECS::BaseEntity molEntity = allSystems.front();
	REQUIRE( molEntity != App::Core::ECS::INVALID_ENTITY );
	REQUIRE( SCENE().getAllSceneItems().find( molEntity ) != SCENE().getAllSceneItems().end() );

	App::Core::ECS::ViewIterator findFailIt = SCENE().getAllSceneItems().find( App::Core::ECS::BaseEntity( 666 ) );
	REQUIRE( findFailIt == SCENE().getAllSceneItems().end() );

	SCENE().update( 0.015f );
};

TEST_CASE( "VTX_APP - Full sequence", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	class CallbackTest
	{
	  public:
		bool checked = false;
	};

	const std::string systemPathname = App::Test::Util::App::MOLECULE_TEST_NAME + ".mmtf";

	App::Fixture app;

	CallbackTest addSceneItemTest = CallbackTest();
	SCENE().onSceneItemAdded += [ &addSceneItemTest ]( const Component::Scene::SceneItemComponent & p_sceneItem )
	{ addSceneItemTest.checked = !p_sceneItem.getName().empty(); };

	// Create System
	const FilePath				systemPath = App::Filesystem::getInternalDataDir() / systemPathname;
	Action::Scene::LoadSystem openAction	 = Action::Scene::LoadSystem( systemPath );
	openAction.execute();

	REQUIRE( addSceneItemTest.checked );

	// Pick first System
	REQUIRE( SCENE().getItemCount() == 1 );

	App::Core::ECS::BaseEntity systemEntity = SCENE().getItem( 0 );
	REQUIRE( ECS_REGISTRY().isValid( systemEntity ) );

	systemEntity = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	REQUIRE( ECS_REGISTRY().isValid( systemEntity ) );

	Component::Scene::SceneItemComponent & sceneItem
		= ECS_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( systemEntity );

	CallbackTest renameTest = CallbackTest();

	sceneItem.onName += [ &renameTest ]( const std::string_view p_name ) { renameTest.checked = true; };
	sceneItem.setName( "Zouzou" );

	REQUIRE( sceneItem.getName() == "Zouzou" );
	REQUIRE( renameTest.checked );

	const App::Core::ECS::View view = SCENE().getAllSceneItemsOfType<Component::Chemistry::System>();
	REQUIRE( view.size() == 1 );

	// const Component::Chemistry::System & systemComponent
	//	= ECS_REGISTRY().getComponent<Component::Chemistry::System>( systemEntity );

	// const Component::Render::ProxySystem & gpuProxyComponent
	//	= ECS_REGISTRY().getComponent<Component::Render::ProxySystem>( systemEntity );

	// REQUIRE( gpuProxyComponent.getProxy().atomNames == &systemComponent.getSystemStruct().atomNames );
}

TEST_CASE( "VTX_APP - Benchmark", "[.][perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	const std::string systemPathname = App::Test::Util::App::MOLECULE_TEST_NAME + ".mmtf";

	// Create Scene
	App::Fixture app;

	// Create System
	const FilePath systemPath = App::Filesystem::getInternalDataDir() / systemPathname;

	Action::Scene::LoadSystem openAction = Action::Scene::LoadSystem( systemPath );
	openAction.execute();
	BENCHMARK( "Open systems" ) { openAction.execute(); };

	int i = 0;
	BENCHMARK( "Get" )
	{
		App::Core::ECS::BaseEntity systemEntity = SCENE().getItem( i );
		i++;
	};

	App::Core::ECS::BaseEntity			 systemEntity = SCENE().getItem( 0 );
	const Component::Chemistry::System & system
		= ECS_REGISTRY().getComponent<const Component::Chemistry::System>( systemEntity );

	const Component::Chemistry::Residue & residue = *system.getResidue( 0 );

	size_t sumAtomCount = 0;
	BENCHMARK( "Data access" )
	{
		for ( int i = 0; i < 100000; i++ )
			sumAtomCount += residue.getAtomCount();
	};

	BENCHMARK( "View all" )
	{
		App::Core::ECS::View view = SCENE().getAllSceneItemsOfType<Component::Chemistry::System>();
	};
}
