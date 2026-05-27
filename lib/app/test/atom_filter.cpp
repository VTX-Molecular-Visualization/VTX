
#include <app/system/atom_filter.hpp>
#include <app/system/selection.hpp>
#include <app/system/visibility.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - AtomFilter - Visible", "[atomFilter][Visible]" )
{
	using namespace VTX::App::System;
	Visibility vis { VTX::Util::Math::BitSet { 10 } };
	vis.atoms.set( 2 );
	vis.atoms.set( 7 );
	atomFilters::Visible filter( vis );

	CHECK( filter( 1 ) == false );
	CHECK( filter( 2 ) == true );
	CHECK( filter( 5 ) == false );
	CHECK( filter( 7 ) == true );
}

TEST_CASE( "VTX_APP - AtomFilter - Visible", "[atomFilter][Selected]" )
{
	using namespace VTX::App::System;
	Selection vis { VTX::Util::Math::BitSet { 10 } };
	vis.atoms.set( 2 );
	vis.atoms.set( 7 );
	atomFilters::Selected filter( vis );

	CHECK( filter( 1 ) == false );
	CHECK( filter( 2 ) == true );
	CHECK( filter( 5 ) == false );
	CHECK( filter( 7 ) == true );
}
