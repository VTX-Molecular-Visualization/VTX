#include <catch2/catch_test_macros.hpp>
#include <util/math/range.hpp>

TEST_CASE( "Util::Math::Range", "[unit]" )
{
	using namespace VTX;

	Util::Math::Range<size_t> range;
	CHECK( !range.isValid() );

	range = Util::Math::Range<size_t>( 6 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 6 );
	CHECK( range.getCount() == 1 );
	CHECK( range.getLast() == 6 );

	range = Util::Math::Range<size_t>( 50, 5 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 50 );
	CHECK( range.getCount() == 5 );
	CHECK( range.getLast() == 54 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getCount() == 11 );
	CHECK( range.getLast() == 20 );

	range.add( 3 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 23 );

	range.remove( 8 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.setFirst( 15 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 15 );
	CHECK( range.getLast() == 20 );

	range.setLast( 15 );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 15 );
	CHECK( range.getLast() == 15 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 12 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 5 );
	CHECK( range.getLast() == 20 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 19, 27 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 10 );
	CHECK( range.getLast() == 27 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 5, 25 ) );
	CHECK( range.isValid() );
	CHECK( range.getFirst() == 5 );
	CHECK( range.getLast() == 25 );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	range.merge( Util::Math::Range<size_t>::createFirstLast( 25, 35 ) );
	CHECK( !range.isValid() );

	range = Util::Math::Range<size_t>::createFirstLast( 10, 20 );
	CHECK( range.contains( 10 ) );
	CHECK( range.contains( 15 ) );
	CHECK( range.contains( 20 ) );
	CHECK( !range.contains( 9 ) );
	CHECK( !range.contains( 21 ) );

	CHECK( range.contains( { 10, 12, 18 } ) );
	CHECK( !range.contains( { 10, 12, 18, 22 } ) );

	CHECK( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 20 ) ) );
	CHECK( range.contains( Util::Math::Range<size_t>::createFirstLast( 10, 15 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 9, 24 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 1, 5 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 12, 24 ) ) );
	CHECK( !range.contains( Util::Math::Range<size_t>::createFirstLast( 500, 5000 ) ) );

	CHECK( range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
							 Util::Math::Range<size_t>::createFirstLast( 14, 18 ) } ) );
	CHECK( !range.contains( { Util::Math::Range<size_t>::createFirstLast( 10, 12 ),
							  Util::Math::Range<size_t>::createFirstLast( 14, 18 ),
							  Util::Math::Range<size_t>::createFirstLast( 19, 22 ) } ) );
};
