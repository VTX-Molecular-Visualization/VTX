#include <catch2/catch_test_macros.hpp>
#include <util/math/grid.hpp>

TEST_CASE( "Grid computes cell count", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid( VTX::Vec3f( -1.f, 2.f, 4.f ), 2.f, VTX::Vec3u( 3u, 4u, 5u ) );

	REQUIRE( grid.getCellCount() == 60 );
}

TEST_CASE( "Grid converts world position to grid position", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid(
		VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ), VTX::Vec3u( 3u, 4u, 5u )
	);

	const VTX::Vec3u position = grid.gridPosition( VTX::Vec3f( 3.2f, 10.5f, 20.1f ) );

	REQUIRE( position.x == 2u );
	REQUIRE( position.y == 2u );
	REQUIRE( position.z == 2u );
}

TEST_CASE( "Grid converts grid position to hash", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid( VTX::Vec3f( 0.f ), 1.f, VTX::Vec3u( 4u, 5u, 6u ) );

	const size_t hash = grid.gridHash( VTX::Vec3u( 2u, 3u, 1u ) );

	REQUIRE( hash == 34 );
	REQUIRE( grid.gridPosition( hash ) == VTX::Vec3u( 2u, 3u, 1u ) );
}

TEST_CASE( "Grid returns world position at cell center", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid(
		VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ), VTX::Vec3u( 3u, 4u, 5u )
	);

	const VTX::Vec3f position = grid.worldPosition( VTX::Vec3u( 2u, 1u, 3u ) );

	REQUIRE( position.x == 4.f );
	REQUIRE( position.y == 8.f );
	REQUIRE( position.z == 32.f );
}
