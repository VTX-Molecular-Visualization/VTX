#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Scene", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_0" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_1" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_2" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_3" );
	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" ).setName( "1AGA_4" );

	CHECK( SCENE().getItem( 0 ) == SCENE().getItem( "1AGA_0" ) );
	CHECK( SCENE().getItem( 1 ) == SCENE().getItem( "1AGA_1" ) );
	CHECK( SCENE().getItem( 2 ) == SCENE().getItem( "1AGA_2" ) );
	CHECK( SCENE().getItem( 3 ) == SCENE().getItem( "1AGA_3" ) );
	CHECK( SCENE().getItem( 4 ) == SCENE().getItem( "1AGA_4" ) );

	SCENE().changeItemIndex( SCENE().getItem( "1AGA_2" ), 4 );
	CHECK( SCENE().getItem( 0 ) == SCENE().getItem( "1AGA_0" ) );
	CHECK( SCENE().getItem( 1 ) == SCENE().getItem( "1AGA_1" ) );
	CHECK( SCENE().getItem( 2 ) == SCENE().getItem( "1AGA_3" ) );
	CHECK( SCENE().getItem( 3 ) == SCENE().getItem( "1AGA_4" ) );
	CHECK( SCENE().getItem( 4 ) == SCENE().getItem( "1AGA_2" ) );

	SCENE().changeItemsIndex( { SCENE().getItem( "1AGA_3" ), SCENE().getItem( "1AGA_4" ) }, 0 );
	CHECK( SCENE().getItem( 0 ) == SCENE().getItem( "1AGA_3" ) );
	CHECK( SCENE().getItem( 1 ) == SCENE().getItem( "1AGA_4" ) );
	CHECK( SCENE().getItem( 2 ) == SCENE().getItem( "1AGA_0" ) );
	CHECK( SCENE().getItem( 3 ) == SCENE().getItem( "1AGA_1" ) );
	CHECK( SCENE().getItem( 4 ) == SCENE().getItem( "1AGA_2" ) );

	SCENE().changeItemsIndex( { SCENE().getItem( "1AGA_3" ), SCENE().getItem( "1AGA_4" ) }, 5 );
	CHECK( SCENE().getItem( 0 ) == SCENE().getItem( "1AGA_0" ) );
	CHECK( SCENE().getItem( 1 ) == SCENE().getItem( "1AGA_1" ) );
	CHECK( SCENE().getItem( 2 ) == SCENE().getItem( "1AGA_2" ) );
	CHECK( SCENE().getItem( 3 ) == SCENE().getItem( "1AGA_3" ) );
	CHECK( SCENE().getItem( 4 ) == SCENE().getItem( "1AGA_4" ) );

	std::vector<App::Core::ECS::BaseEntity> items
		= { SCENE().getItem( "1AGA_3" ), SCENE().getItem( "1AGA_0" ), SCENE().getItem( "1AGA_4" ) };

	SCENE().sortItemsBySceneIndex( items );

	CHECK( Test::Util::App::checkItemOrder( SCENE(), items ) );
}
