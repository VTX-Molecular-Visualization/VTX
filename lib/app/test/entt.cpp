#include "util/app.hpp"
// #include <app/ecs/component/molecule_component.hpp>
#include <app/action/scene.hpp>
#include <app/application/ecs/registry_manager.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <app/entity/scene/molecule_entity.hpp>
#include <app/filesystem.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <renderer/proxy/molecule.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

TEST_CASE( "VTX_APP - Views", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Test::Util::App::loadTestMolecule();

	App::Core::ECS::View view1Element = SCENE().getAllSceneItems();
	REQUIRE( view1Element.size() == 1 );

	const App::Component::Scene::SceneItemComponent & sceneItemComponent
		= view1Element.getComponent<App::Component::Scene::SceneItemComponent>( view1Element.front() );
	REQUIRE( sceneItemComponent.getName() == App::Test::Util::App::MOLECULE_TEST_NAME );

	App::Core::ECS::View allMolecules = SCENE().getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	REQUIRE( allMolecules.size() == 1 );

	App::Core::ECS::BaseEntity molEntity = allMolecules.front();
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

	const std::string moleculePathname = App::Test::Util::App::MOLECULE_TEST_NAME + ".mmtf";

	Test::Util::App::initApp();

	CallbackTest addSceneItemTest = CallbackTest();
	SCENE().onSceneItemAdded += [ &addSceneItemTest ]( const Component::Scene::SceneItemComponent & p_sceneItem )
	{ addSceneItemTest.checked = !p_sceneItem.getName().empty(); };

	// Create MoleculeEntity
	const FilePath				moleculePath = App::Filesystem::getInternalDataDir() / moleculePathname;
	Action::Scene::LoadMolecule openAction	 = Action::Scene::LoadMolecule( moleculePath );
	openAction.execute();

	REQUIRE( addSceneItemTest.checked );

	// Pick first Molecule
	REQUIRE( SCENE().getItemCount() == 1 );

	App::Core::ECS::BaseEntity moleculeEntity = SCENE().getItem( 0 );
	REQUIRE( MAIN_REGISTRY().isValid( moleculeEntity ) );

	moleculeEntity = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	REQUIRE( MAIN_REGISTRY().isValid( moleculeEntity ) );

	Component::Scene::SceneItemComponent & sceneItem
		= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( moleculeEntity );

	CallbackTest renameTest = CallbackTest();

	sceneItem.onName += [ &renameTest ]( const std::string & p_name ) { renameTest.checked = true; };
	sceneItem.setName( "Zouzou" );

	REQUIRE( sceneItem.getName() == "Zouzou" );
	REQUIRE( renameTest.checked );

	const App::Core::ECS::View view = SCENE().getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	REQUIRE( view.size() == 1 );

	// const Component::Chemistry::Molecule & moleculeComponent
	//	= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( moleculeEntity );

	// const Component::Render::ProxyMolecule & gpuProxyComponent
	//	= MAIN_REGISTRY().getComponent<Component::Render::ProxyMolecule>( moleculeEntity );

	// REQUIRE( gpuProxyComponent.getProxy().atomNames == &moleculeComponent.getMoleculeStruct().atomNames );
}

TEST_CASE( "VTX_APP - Benchmark", "[.][perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	const std::string moleculePathname = App::Test::Util::App::MOLECULE_TEST_NAME + ".mmtf";

	// Create Scene
	Test::Util::App::initApp();

	// Create MoleculeEntity
	const FilePath moleculePath = App::Filesystem::getInternalDataDir() / moleculePathname;

	Action::Scene::LoadMolecule openAction = Action::Scene::LoadMolecule( moleculePath );
	openAction.execute();
	BENCHMARK( "Open molecules" ) { openAction.execute(); };

	int i = 0;
	BENCHMARK( "Get" )
	{
		App::Core::ECS::BaseEntity moleculeEntity = SCENE().getItem( i );
		i++;
	};

	App::Core::ECS::BaseEntity			   moleculeEntity = SCENE().getItem( 0 );
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
		App::Core::ECS::View view = SCENE().getAllSceneItemsOfType<Component::Chemistry::Molecule>();
	};
}