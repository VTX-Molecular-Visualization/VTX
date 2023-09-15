module;

// #include <app/ecs/component/molecule_component.hpp>
#include <app/application/ecs/registry_manager.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/entity/scene/molecule_entity.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
// #include <core/old/struct/atom.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <io/internal/filesystem.hpp>
#include <renderer/gl/struct_proxy_molecule.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

export module Test.Entt;

import Test.Utils;

TEST_CASE( "VTX_APP - Views", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	initApp();
	loadTestMolecule();

	Application::Scene & scene = VTXApp::get().getScene();

	App::Core::ECS::View view1Element = scene.getAllSceneItems();
	REQUIRE( view1Element.size() == 1 );

	const App::Component::Scene::SceneItemComponent & sceneItemComponent
		= view1Element.getComponent<App::Component::Scene::SceneItemComponent>( view1Element.front() );
	REQUIRE( sceneItemComponent.getName() == MOLECULE_TEST_NAME );

	App::Core::ECS::View allMolecules = scene.getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	REQUIRE( allMolecules.size() == 1 );

	App::Core::ECS::BaseEntity molEntity = allMolecules.front();
	REQUIRE( molEntity != App::Core::ECS::INVALID_ENTITY );
	REQUIRE( scene.getAllSceneItems().find( molEntity ) != scene.getAllSceneItems().end() );

	App::Core::ECS::ViewIterator findFailIt = scene.getAllSceneItems().find( App::Core::ECS::BaseEntity( 666 ) );
	REQUIRE( findFailIt == scene.getAllSceneItems().end() );

	scene.update( 0.015f );
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

	const std::string moleculePathname = MOLECULE_TEST_NAME + ".mmtf";

	initApp();

	// Create Scene
	Application::Scene & scene			  = VTXApp::get().getScene();
	CallbackTest		 addSceneItemTest = CallbackTest();
	scene.onSceneItemAddedCallback().addCallback(
		&addSceneItemTest,
		[ &addSceneItemTest ]( Component::Scene::SceneItemComponent & p_sceneItem )
		{ addSceneItemTest.checked = !p_sceneItem.getName().empty(); } );

	// Create MoleculeEntity
	const FilePath				moleculePath = IO::Internal::Filesystem::getInternalDataDir() / moleculePathname;
	Internal::Action::ECS::Open openAction	 = Internal::Action::ECS::Open( moleculePath );
	openAction.execute();

	REQUIRE( addSceneItemTest.checked );

	// Pick first Molecule
	REQUIRE( scene.getItemCount() == 1 );

	App::Core::ECS::BaseEntity moleculeEntity = scene.getItem( 0 );
	REQUIRE( MAIN_REGISTRY().isValid( moleculeEntity ) );

	moleculeEntity = scene.getItem( MOLECULE_TEST_NAME );
	REQUIRE( MAIN_REGISTRY().isValid( moleculeEntity ) );

	Component::Scene::SceneItemComponent & sceneItem
		= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( moleculeEntity );

	CallbackTest renameTest = CallbackTest();

	sceneItem.onNameChange().addCallback(
		&renameTest, [ &renameTest ]( const std::string & p_name ) { renameTest.checked = true; } );
	sceneItem.setName( "Zouzou" );

	REQUIRE( sceneItem.getName() == "Zouzou" );
	REQUIRE( renameTest.checked );

	const App::Core::ECS::View view = scene.getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	REQUIRE( view.size() == 1 );

	const Renderer::GL::StructProxyMolecule & gpuProxyComponent
		= MAIN_REGISTRY().getComponent<Renderer::GL::StructProxyMolecule>( moleculeEntity );

	REQUIRE( gpuProxyComponent.atomIds != nullptr );
	REQUIRE( ( ( *gpuProxyComponent.atomIds )[ 2 ] ) == uint( 2 ) );
}

TEST_CASE( "VTX_APP - Benchmark", "[.][perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	const std::string moleculePathname = MOLECULE_TEST_NAME + ".mmtf";

	// Create Scene
	initApp();

	const Application::Scene & scene = VTXApp::get().getScene();

	// Create MoleculeEntity
	const FilePath moleculePath = IO::Internal::Filesystem::getInternalDataDir() / moleculePathname;

	Internal::Action::ECS::Open openAction = Internal::Action::ECS::Open( moleculePath );
	openAction.execute();
	BENCHMARK( "Open molecules" ) { openAction.execute(); };

	int i = 0;
	BENCHMARK( "Get" )
	{
		App::Core::ECS::BaseEntity moleculeEntity = scene.getItem( i );
		i++;
	};

	App::Core::ECS::BaseEntity			   moleculeEntity = scene.getItem( 0 );
	const Component::Chemistry::Molecule & molecule
		= MAIN_REGISTRY().getComponent<const Component::Chemistry::Molecule>( moleculeEntity );

	const Component::Chemistry::Residue & residue = *molecule.getResidue( 0 );

	size_t sumAtomCount = 0;
	BENCHMARK( "Data access" )
	{
		for ( int i = 0; i < 100000; i++ )
			sumAtomCount += residue.getAtomCount();
	};

	BENCHMARK( "View all" )
	{
		App::Core::ECS::View view = scene.getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	};
}
