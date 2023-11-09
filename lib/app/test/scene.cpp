#include "util/app.hpp"
#include <app/application/ecs/registry_manager.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Scene", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Application::Scene & scene = VTXApp::get().getScene();

	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	scene.getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_0" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	scene.getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_1" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	scene.getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_2" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	scene.getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_3" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	scene.getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_4" );

	CHECK( scene.getItem( 0 ) == scene.getItem( "1AGA_0" ) );
	CHECK( scene.getItem( 1 ) == scene.getItem( "1AGA_1" ) );
	CHECK( scene.getItem( 2 ) == scene.getItem( "1AGA_2" ) );
	CHECK( scene.getItem( 3 ) == scene.getItem( "1AGA_3" ) );
	CHECK( scene.getItem( 4 ) == scene.getItem( "1AGA_4" ) );

	scene.changeItemIndex( scene.getItem( "1AGA_2" ), 4 );
	CHECK( scene.getItem( 0 ) == scene.getItem( "1AGA_0" ) );
	CHECK( scene.getItem( 1 ) == scene.getItem( "1AGA_1" ) );
	CHECK( scene.getItem( 2 ) == scene.getItem( "1AGA_3" ) );
	CHECK( scene.getItem( 3 ) == scene.getItem( "1AGA_4" ) );
	CHECK( scene.getItem( 4 ) == scene.getItem( "1AGA_2" ) );

	scene.changeItemsIndex( { scene.getItem( "1AGA_3" ), scene.getItem( "1AGA_4" ) }, 0 );
	CHECK( scene.getItem( 0 ) == scene.getItem( "1AGA_3" ) );
	CHECK( scene.getItem( 1 ) == scene.getItem( "1AGA_4" ) );
	CHECK( scene.getItem( 2 ) == scene.getItem( "1AGA_0" ) );
	CHECK( scene.getItem( 3 ) == scene.getItem( "1AGA_1" ) );
	CHECK( scene.getItem( 4 ) == scene.getItem( "1AGA_2" ) );

	scene.changeItemsIndex( { scene.getItem( "1AGA_3" ), scene.getItem( "1AGA_4" ) }, 5 );
	CHECK( scene.getItem( 0 ) == scene.getItem( "1AGA_0" ) );
	CHECK( scene.getItem( 1 ) == scene.getItem( "1AGA_1" ) );
	CHECK( scene.getItem( 2 ) == scene.getItem( "1AGA_2" ) );
	CHECK( scene.getItem( 3 ) == scene.getItem( "1AGA_3" ) );
	CHECK( scene.getItem( 4 ) == scene.getItem( "1AGA_4" ) );

	std::vector<App::Core::ECS::BaseEntity> items
		= { scene.getItem( "1AGA_3" ), scene.getItem( "1AGA_0" ), scene.getItem( "1AGA_4" ) };

	scene.sortItemsBySceneIndex( items );

	CHECK( Test::Util::App::checkItemOrder( scene, items ) );
}
