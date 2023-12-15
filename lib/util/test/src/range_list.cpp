#include <catch2/catch_test_macros.hpp>
#include <util/algorithm/range.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <vector>

TEST_CASE( "Util::Math::RangeList", "[unit]" )
{
	using namespace VTX;

	Util::Math::RangeList<size_t> rangeList
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
										   Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 9, 10 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 8, 14 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 51, 55 ) );
	rangeList.addRange( Util::Math::Range<size_t>::createFirstLast( 6, 7 ) );

	int itemCount = 0;
	for ( size_t index : rangeList )
		itemCount++;

	CHECK( itemCount == 22 );

	rangeList = Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
												 Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
												 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 1, 4 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 10, 14 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 6, 7 ) );
	rangeList.removeRange( Util::Math::Range<size_t>::createFirstLast( 1, 18 ) );

	itemCount = 0;
	for ( size_t index : rangeList )
		itemCount++;

	CHECK( itemCount == 3 );

	rangeList = Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
												 Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
												 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	CHECK( rangeList.contains( 7 ) );
	CHECK( !rangeList.contains( 10 ) );

	CHECK( rangeList.contains( { 7, 13, 50 } ) );
	CHECK( !rangeList.contains( { 7, 13, 50, 52 } ) );

	CHECK( rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 20 ) ) );
	CHECK( !rangeList.contains( Util::Math::Range<size_t>::createFirstLast( 18, 50 ) ) );

	CHECK( rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
								 Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
								 Util::Math::Range<size_t>::createFirstLast( 12, 14 ),
								 Util::Math::Range<size_t>::createFirstLast( 18, 20 ) } ) );

	CHECK( !rangeList.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
								  Util::Math::Range<size_t>::createFirstLast( 50, 50 ),
								  Util::Math::Range<size_t>::createFirstLast( 7, 14 ) } ) );
};

TEST_CASE( "Util::Math::RangeList - Operators", "[unit]" )
{
	using namespace VTX;

	Util::Math::RangeList<size_t> rangeListA
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
										   Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	Util::Math::RangeList<size_t> rangeListB
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
										   Util::Math::Range<size_t>::createFirstLast( 4, 5 ),
										   Util::Math::Range<size_t>::createFirstLast( 7, 14 ),
										   Util::Math::Range<size_t>::createFirstLast( 18, 19 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	// Check Merges
	Util::Math::RangeList<size_t> rangeListRes1 = Util::Algorithm::Range::merge( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 20 ),
									 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } ) );

	Util::Math::RangeList<size_t> rangeListRes2 = Util::Algorithm::Range::merge( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::mergeInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Substract
	rangeListRes1 = Util::Algorithm::Range::substract( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 6, 6 ),
									 Util::Math::Range<size_t>::createFirstLast( 15, 17 ),
									 Util::Math::Range<size_t>::createFirstLast( 20, 20 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::substract( rangeListB, rangeListA );
	CHECK( rangeListRes2.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 4 ),
									 Util::Math::Range<size_t>::createFirstLast( 9, 11 ) } ) );

	rangeListRes2 = rangeListA;
	Util::Algorithm::Range::substractInSitu( rangeListRes2, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Intersect
	rangeListRes1 = Util::Algorithm::Range::intersect( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 5, 5 ),
									 Util::Math::Range<size_t>::createFirstLast( 7, 8 ),
									 Util::Math::Range<size_t>::createFirstLast( 12, 14 ),
									 Util::Math::Range<size_t>::createFirstLast( 18, 19 ),
									 Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::intersect( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::intersectInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Exclusive
	rangeListRes1 = Util::Algorithm::Range::exclusive( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Util::Math::Range<size_t>::createFirstLast( 0, 1 ),
									 Util::Math::Range<size_t>::createFirstLast( 4, 4 ),
									 Util::Math::Range<size_t>::createFirstLast( 15, 17 ),
									 Util::Math::Range<size_t>::createFirstLast( 20, 20 ) } ) );

	rangeListRes2 = Util::Algorithm::Range::exclusive( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	Util::Algorithm::Range::exclusiveInSitu( rangeListRes1, rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );
}

TEST_CASE( "Util::Math::RangeList - FillVector", "[unit]" )
{
	using namespace VTX;

	std::vector<bool> vecIndexes = std::vector<bool>();
	vecIndexes.resize( 54, false );
	vecIndexes[ 5 ]	 = true;
	vecIndexes[ 6 ]	 = true;
	vecIndexes[ 7 ]	 = true;
	vecIndexes[ 8 ]	 = true;
	vecIndexes[ 12 ] = true;
	vecIndexes[ 13 ] = true;
	vecIndexes[ 14 ] = true;
	vecIndexes[ 15 ] = true;
	vecIndexes[ 16 ] = true;
	vecIndexes[ 17 ] = true;
	vecIndexes[ 18 ] = true;
	vecIndexes[ 19 ] = true;
	vecIndexes[ 20 ] = true;
	vecIndexes[ 50 ] = true;

	const Util::Math::RangeList<size_t> generatedRangeList
		= Util::Algorithm::Range::generateIndexRangeList( vecIndexes, []( const bool & p_value ) { return p_value; } );

	const Util::Math::RangeList<size_t> expectedRangeList
		= Util::Math::RangeList<size_t>( { Util::Math::Range<size_t>::createFirstLast( 5, 8 ),
										   Util::Math::Range<size_t>::createFirstLast( 12, 20 ),
										   Util::Math::Range<size_t>::createFirstLast( 50, 50 ) } );

	CHECK( generatedRangeList == expectedRangeList );
}
