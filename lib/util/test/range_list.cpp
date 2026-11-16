#include <catch2/catch_test_macros.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <vector>

TEST_CASE( "Util::Math::RangeList operations", "[unit]" )
{
	CHECK( (
		[]
		{
			using R = VTX::Util::Math::Range<int>;
			using L = VTX::Util::Math::RangeList<int>;
			L a { R( 1, 5 ), R( 8, 10 ) };
			a.addRange( R( 4, 9 ) );
			a.removeRange( R( 3, 7 ) );
			const L b { R( 2, 8 ) };
			if ( L::intersect( a, b ) != L { R( 2, 3 ), R( 7, 8 ) } || L::merge( a, b ) != L( R( 1, 10 ) )
				 || L::substract( a, b ) != L { R( 1, 2 ), R( 8, 10 ) }
				 || L::exclusive( a, b ) != L { R( 1, 2 ), R( 3, 7 ), R( 8, 10 ) } || a.invert() != L( R( 3, 7 ) ) )
			{
				return false;
			}
			int sum = 0;
			for ( const int value : a )
			{
				sum += value;
			}
			return sum == 27 && a.toVector<int>() == std::vector<int> { 1, 2, 7, 8, 9 }
				   && R( 1, 4 ).toStdVector() == std::vector<int> { 1, 2, 3 };
		}() ) );
}

TEST_CASE( "Util::Math::RangeList", "[unit]" )
{
	using namespace VTX;
	using namespace Util::Math;

	RangeList<size_t> rangeList = { Range<size_t>( 5, 8 ), Range<size_t>( 12, 20 ), Range<size_t>( 50, 51 ) };

	const RangeList<size_t> test = { { 1, 2 }, { 1, 2 } };

	rangeList.addRange( Range<size_t>( 9, 10 ) );
	rangeList.addRange( Range<size_t>( 8, 14 ) );
	rangeList.addRange( Range<size_t>( 51, 55 ) );
	rangeList.addRange( Range<size_t>( 6, 7 ) );

	int itemCount = 0;
	for ( const size_t index : rangeList )
	{
		itemCount++;
	}

	CHECK( itemCount == 20 );

	rangeList = { Range<size_t>( 5, 8 ), Range<size_t>( 12, 20 ), Range<size_t>( 50, 50 ) };
	rangeList.removeRange( Range<size_t>( 1, 4 ) );
	rangeList.removeRange( Range<size_t>( 10, 14 ) );
	rangeList.removeRange( Range<size_t>( 6, 7 ) );
	rangeList.removeRange( Range<size_t>( 1, 18 ) );

	itemCount = 0;
	for ( const size_t index : rangeList )
	{
		itemCount++;
	}

	CHECK( itemCount == 2 );

	rangeList = { Range<size_t>( 5, 8 ), Range<size_t>( 12, 20 ), Range<size_t>( 50, 50 ) };

	CHECK( rangeList.contains( 7 ) );
	CHECK( !rangeList.contains( 10 ) );

	CHECK( !rangeList.contains( { 7, 13, 50 } ) );
	CHECK( !rangeList.contains( { 7, 13, 50, 52 } ) );

	CHECK( rangeList.contains( Range<size_t>( 18, 20 ) ) );
	CHECK( !rangeList.contains( Range<size_t>( 18, 50 ) ) );

	CHECK( rangeList.contains( { Range<size_t>( 5, 8 ), Range<size_t>( 12, 14 ), Range<size_t>( 18, 20 ) } ) );

	CHECK( !rangeList.contains( { Range<size_t>( 5, 8 ), Range<size_t>( 50, 50 ), Range<size_t>( 7, 14 ) } ) );
};

TEST_CASE( "Util::Math::RangeList - Operators", "[unit]" )
{
	using namespace VTX;
	using namespace Util::Math;

	const RangeList<size_t> rangeListA = { { 5, 8 }, Range<size_t>( 12, 20 ), Range<size_t>( 50, 50 ) };

	const RangeList<size_t> rangeListB = { Range<size_t>( 0, 2 ),
										   Range<size_t>( 4, 6 ),
										   Range<size_t>( 7, 14 ),
										   Range<size_t>( 18, 22 ),
										   Range<size_t>( 50, 51 ) };

	// Check Merges
	RangeList<size_t> rangeListRes1 = RangeList<size_t>::merge( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Range<size_t>( 0, 2 ), Range<size_t>( 8, 12 ), Range<size_t>( 50, 51 ) } ) );

	RangeList<size_t> rangeListRes2 = RangeList<size_t>::merge( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	rangeListRes1.mergeInPlace( rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Substract
	rangeListRes1 = RangeList<size_t>::substract( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains( { Range<size_t>( 6, 7 ), Range<size_t>( 15, 17 ), Range<size_t>( 14, 18 ) } ) );

	rangeListRes2 = RangeList<size_t>::substract( rangeListB, rangeListA );
	CHECK( rangeListRes2.contains( { Range<size_t>( 0, 1 ), Range<size_t>( 4, 4 ), Range<size_t>( 9, 11 ) } ) );

	rangeListRes2 = rangeListA;
	rangeListRes2.substractInPlace( rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Intersect
	rangeListRes1 = RangeList<size_t>::intersect( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains(
		{ Range<size_t>( 5, 5 ), Range<size_t>( 7, 8 ), Range<size_t>( 12, 14 ), Range<size_t>( 18, 19 ) } ) );

	rangeListRes2 = RangeList<size_t>::intersect( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	rangeListRes1.intersectInPlace( rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );

	// Check Exclusive
	rangeListRes1 = RangeList<size_t>::exclusive( rangeListA, rangeListB );
	CHECK( rangeListRes1.contains(
		{ Range<size_t>( 0, 1 ), Range<size_t>( 4, 4 ), Range<size_t>( 15, 17 ), Range<size_t>( 20, 20 ) } ) );

	rangeListRes2 = RangeList<size_t>::exclusive( rangeListB, rangeListA );
	CHECK( rangeListRes1 == rangeListRes2 );

	rangeListRes1 = rangeListA;
	rangeListRes1.exclusiveInPlace( rangeListB );
	CHECK( rangeListRes1 == rangeListRes2 );
}

TEST_CASE( "Util::Math::RangeList - FillVector", "[unit]" )
{
	using namespace VTX;
	using namespace Util::Math;

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

	/*
	const RangeList<size_t> generatedRangeList
		= RangeList<size_t>::generateIndexRangeList( vecIndexes, []( const bool & p_value ) { return p_value; } );

	const RangeList<size_t> expectedRangeList
		= RangeList<size_t>( { Range<size_t>( 5, 8 ), Range<size_t>( 12, 20 ), Range<size_t>( 50, 50 ) } );

	CHECK( generatedRangeList == expectedRangeList );
	*/
}
