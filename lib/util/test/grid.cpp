#include <catch2/catch_test_macros.hpp>
#include <util/math/grid.hpp>

TEST_CASE( "DenseGrid computes cell count", "[util][math][grid]" )
{
	const VTX::Util::Math::DenseGrid grid( VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f ), VTX::Vec3u( 3u, 4u, 5u ) );

	REQUIRE( grid.getCellCount() == 60 );
}

TEST_CASE( "DenseGrid converts world position to grid position", "[util][math][grid]" )
{
	const VTX::Util::Math::DenseGrid grid(
		VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ), VTX::Vec3u( 3u, 4u, 5u )
	);

	const VTX::Vec3u position = grid.gridPosition( VTX::Vec3f( 3.2f, 10.5f, 20.1f ) );

	REQUIRE( position.x == 2u );
	REQUIRE( position.y == 2u );
	REQUIRE( position.z == 2u );
}

TEST_CASE( "DenseGrid converts grid position to dense index", "[util][math][grid]" )
{
	const VTX::Util::Math::DenseGrid grid( VTX::Vec3f( 0.f ), VTX::Vec3f( 1.f ), VTX::Vec3u( 4u, 5u, 6u ) );

	const size_t index = grid.denseIndex( VTX::Vec3u( 2u, 3u, 1u ) );

	REQUIRE( index == 34 );
	REQUIRE( grid.gridPosition( index ) == VTX::Vec3u( 2u, 3u, 1u ) );
}

TEST_CASE( "DenseGrid returns world position at cell center", "[util][math][grid]" )
{
	const VTX::Util::Math::DenseGrid grid(
		VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ), VTX::Vec3u( 3u, 4u, 5u )
	);

	const VTX::Vec3f position = grid.worldPosition( VTX::Vec3u( 2u, 1u, 3u ) );

	REQUIRE( position.x == 4.f );
	REQUIRE( position.y == 8.f );
	REQUIRE( position.z == 32.f );
}

TEST_CASE( "Grid computes cell count", "[util][math][grid]" )
{
	VTX::Util::Math::Grid<int> grid;

	REQUIRE( grid.getCellCount() == 0 );

	grid.add( 1, VTX::Vec3i( 0, 0, 0 ) );
	grid.add( 2, VTX::Vec3i( 0, 0, 0 ) );
	grid.add( 3, VTX::Vec3i( 1, 0, 0 ) );

	REQUIRE( grid.getCellCount() == 2 );
}

TEST_CASE( "Grid converts world position to grid position", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid( VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ) );

	const VTX::Vec3i position = grid.gridPosition( VTX::Vec3f( 3.2f, -1.5f, 20.1f ) );

	REQUIRE( position.x == 2 );
	REQUIRE( position.y == -1 );
	REQUIRE( position.z == 2 );
}

TEST_CASE( "Grid updates occupied cell bounds", "[util][math][grid]" )
{
	VTX::Util::Math::Grid<int> grid;

	grid.add( 1, VTX::Vec3i( -2, 0, 1 ) );
	grid.add( 2, VTX::Vec3i( 3, -1, 1 ) );

	REQUIRE( grid.getMinCell() == VTX::Vec3i( -2, -1, 1 ) );
	REQUIRE( grid.getMaxCell() == VTX::Vec3i( 3, 0, 1 ) );
	REQUIRE( grid.getSize() == VTX::Vec3u( 6u, 2u, 1u ) );
}

TEST_CASE( "Grid converts cell position to dense index", "[util][math][grid]" )
{
	VTX::Util::Math::Grid<int> grid;

	grid.add( 1, VTX::Vec3i( -2, -1, 1 ) );
	grid.add( 2, VTX::Vec3i( 3, 0, 1 ) );

	REQUIRE( grid.denseIndex( VTX::Vec3i( -2, -1, 1 ) ) == 0 );
	REQUIRE( grid.denseIndex( VTX::Vec3i( -1, -1, 1 ) ) == 1 );
	REQUIRE( grid.denseIndex( VTX::Vec3i( -2, 0, 1 ) ) == 6 );
	REQUIRE( grid.denseIndex( VTX::Vec3i( 3, 0, 1 ) ) == 11 );
}

TEST_CASE( "Grid returns world position at cell center", "[util][math][grid]" )
{
	const VTX::Util::Math::Grid<int> grid( VTX::Vec3f( -1.f, 2.f, 4.f ), VTX::Vec3f( 2.f, 4.f, 8.f ) );

	const VTX::Vec3f position = grid.worldPosition( VTX::Vec3i( -2, 1, 3 ) );

	REQUIRE( position.x == -4.f );
	REQUIRE( position.y == 8.f );
	REQUIRE( position.z == 32.f );
}
