#include <catch2/catch_test_macros.hpp>
#include <util/math/range.hpp>

TEST_CASE( "Util::Math::Range", "[unit]" )
{
	using namespace VTX;
	using namespace Util::Math;

	Util::Math::Range<size_t> range;
	CHECK( range.isValid() );
	CHECK( range.getCount() == 0 );

	range = Range<size_t>( 6 );
	CHECK( range.isValid() );
	CHECK( range.first == 6 );
	CHECK( range.getCount() == 1 );
	CHECK( range.last == 7 );

	range = Range<size_t>::fromFirstCount( 50, 5 );
	CHECK( range.isValid() );
	CHECK( range.first == 50 );
	CHECK( range.getCount() == 5 );
	CHECK( range.last == 55 );

	range = Range<size_t>( 10, 20 );
	CHECK( range.isValid() );
	CHECK( range.first == 10 );
	CHECK( range.getCount() == 10 );
	CHECK( range.last == 20 );

	range.add( 3 );
	CHECK( range.isValid() );
	CHECK( range.first == 3 );
	CHECK( range.last == 20 );

	range.remove( 3 );
	range.remove( 19 );
	CHECK( range.isValid() );
	CHECK( range.first == 4 );
	CHECK( range.last == 19 );

	range		= Range<size_t>( 10, 20 );
	range.first = 15;
	CHECK( range.isValid() );
	CHECK( range.first == 15 );
	CHECK( range.last == 20 );

	range.last = 15;
	CHECK( range.isValid() );
	CHECK( range.first == 15 );
	CHECK( range.last == 15 );

	range = Range<size_t>( 10, 20 );
	range.mergeInPlace( Range<size_t>( 5, 12 ) );
	CHECK( range.isValid() );
	CHECK( range.first == 5 );
	CHECK( range.last == 20 );

	range = Range<size_t>( 10, 20 );
	range.mergeInPlace( Range<size_t>( 19, 27 ) );
	CHECK( range.isValid() );
	CHECK( range.first == 10 );
	CHECK( range.last == 27 );

	range = Range<size_t>( 10, 20 );
	range.mergeInPlace( Range<size_t>( 5, 25 ) );
	CHECK( range.isValid() );
	CHECK( range.first == 5 );
	CHECK( range.last == 25 );

	range = Range<size_t>( 10, 20 );
	range.mergeInPlace( Range<size_t>( 20, 35 ) );
	CHECK( range.isValid() );

	range = Range<size_t>( 10, 20 );
	CHECK( range.contains( 10 ) );
	CHECK( range.contains( 15 ) );
	CHECK( !range.contains( 20 ) );
	CHECK( !range.contains( 9 ) );
	CHECK( !range.contains( 21 ) );

	CHECK( range.contains( { 10, 12, 18 } ) );
	CHECK( !range.contains( { 10, 12, 18, 22 } ) );

	CHECK( range.contains( Range<size_t>( 10, 20 ) ) );
	CHECK( range.contains( Range<size_t>( 10, 15 ) ) );
	CHECK( !range.contains( Range<size_t>( 9, 24 ) ) );
	CHECK( !range.contains( Range<size_t>( 1, 5 ) ) );
	CHECK( !range.contains( Range<size_t>( 12, 24 ) ) );
	CHECK( !range.contains( Range<size_t>( 500, 5000 ) ) );

	CHECK( range.contains( { Range<size_t>( 10, 12 ), Range<size_t>( 14, 18 ) } ) );
	CHECK( !range.contains( { Range<size_t>( 10, 12 ), Range<size_t>( 14, 18 ), Range<size_t>( 19, 22 ) } ) );

	for ( const auto v : range ) {}
};
