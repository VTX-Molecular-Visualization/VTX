#include <catch2/catch_test_macros.hpp>
#include <util/math/bitset.hpp>
#include <vector>

using namespace VTX::Util::Math;

TEST_CASE( "BitSet construct empty", "[util][math][bitset]" )
{
	const BitSet bitset( 0 );

	REQUIRE( bitset.size() == 0 );
	REQUIRE( bitset.empty() );
	REQUIRE( bitset.dataSize() == 0 );
	REQUIRE( bitset.none() );
	REQUIRE_FALSE( bitset.any() );
	REQUIRE( bitset.all() );
	REQUIRE( bitset.count() == 0 );
}

TEST_CASE( "BitSet construct non empty", "[util][math][bitset]" )
{
	const BitSet bitset63( 63 );
	const BitSet bitset64( 64 );
	const BitSet bitset65( 65 );
	const BitSet bitset100( 100 );

	REQUIRE( bitset63.size() == 63 );
	REQUIRE( bitset63.dataSize() == 1 );

	REQUIRE( bitset64.size() == 64 );
	REQUIRE( bitset64.dataSize() == 1 );

	REQUIRE( bitset65.size() == 65 );
	REQUIRE( bitset65.dataSize() == 2 );

	REQUIRE( bitset100.size() == 100 );
	REQUIRE( bitset100.dataSize() == 2 );
}

TEST_CASE( "BitSet set reset test single bits", "[util][math][bitset]" )
{
	BitSet bitset( 130 );

	REQUIRE_FALSE( bitset.test( 0 ) );
	REQUIRE_FALSE( bitset.test( 63 ) );
	REQUIRE_FALSE( bitset.test( 64 ) );
	REQUIRE_FALSE( bitset.test( 129 ) );

	bitset.set( 0 );
	bitset.set( 63 );
	bitset.set( 64 );
	bitset.set( 129 );

	REQUIRE( bitset.test( 0 ) );
	REQUIRE( bitset.test( 63 ) );
	REQUIRE( bitset.test( 64 ) );
	REQUIRE( bitset.test( 129 ) );

	REQUIRE( bitset.count() == 4 );
	REQUIRE( bitset.any() );
	REQUIRE_FALSE( bitset.none() );
	REQUIRE_FALSE( bitset.all() );

	bitset.reset( 63 );
	REQUIRE_FALSE( bitset.test( 63 ) );
	REQUIRE( bitset.test( 0 ) );
	REQUIRE( bitset.test( 64 ) );
	REQUIRE( bitset.test( 129 ) );
	REQUIRE( bitset.count() == 3 );
}

TEST_CASE( "BitSet set with boolean value", "[util][math][bitset]" )
{
	BitSet bitset( 10 );

	bitset.set( 3, true );
	REQUIRE( bitset.test( 3 ) );
	REQUIRE( bitset.count() == 1 );

	bitset.set( 3, false );
	REQUIRE_FALSE( bitset.test( 3 ) );
	REQUIRE( bitset.count() == 0 );
}

TEST_CASE( "BitSet clear resets all bits", "[util][math][bitset]" )
{
	BitSet bitset( 130 );

	bitset.set( 0 );
	bitset.set( 63 );
	bitset.set( 64 );
	bitset.set( 129 );

	REQUIRE( bitset.count() == 4 );

	bitset.clear();

	REQUIRE( bitset.none() );
	REQUIRE_FALSE( bitset.any() );
	REQUIRE( bitset.count() == 0 );
	REQUIRE_FALSE( bitset.test( 0 ) );
	REQUIRE_FALSE( bitset.test( 63 ) );
	REQUIRE_FALSE( bitset.test( 64 ) );
	REQUIRE_FALSE( bitset.test( 129 ) );
	REQUIRE_FALSE( bitset.all() );
}

TEST_CASE( "BitSet fill fills only valid bits", "[util][math][bitset]" )
{
	SECTION( "size 1" )
	{
		BitSet bitset( 1 );
		bitset.fill();

		REQUIRE( bitset.all() );
		REQUIRE( bitset.any() );
		REQUIRE_FALSE( bitset.none() );
		REQUIRE( bitset.count() == 1 );
		REQUIRE( bitset.test( 0 ) );
	}

	SECTION( "size 63" )
	{
		BitSet bitset( 63 );
		bitset.fill();

		REQUIRE( bitset.all() );
		REQUIRE( bitset.count() == 63 );
		for ( size_t i = 0; i < 63; ++i )
			REQUIRE( bitset.test( i ) );
	}

	SECTION( "size 64" )
	{
		BitSet bitset( 64 );
		bitset.fill();

		REQUIRE( bitset.all() );
		REQUIRE( bitset.count() == 64 );
		for ( size_t i = 0; i < 64; ++i )
			REQUIRE( bitset.test( i ) );
	}

	SECTION( "size 65" )
	{
		BitSet bitset( 65 );
		bitset.fill();

		REQUIRE( bitset.all() );
		REQUIRE( bitset.count() == 65 );
		for ( size_t i = 0; i < 65; ++i )
			REQUIRE( bitset.test( i ) );
	}

	SECTION( "size 100" )
	{
		BitSet bitset( 100 );
		bitset.fill();

		REQUIRE( bitset.all() );
		REQUIRE( bitset.count() == 100 );
		for ( size_t i = 0; i < 100; ++i )
			REQUIRE( bitset.test( i ) );
	}
}

TEST_CASE( "BitSet all any none", "[util][math][bitset]" )
{
	BitSet bitset( 10 );

	REQUIRE( bitset.none() );
	REQUIRE_FALSE( bitset.any() );
	REQUIRE_FALSE( bitset.all() );

	bitset.set( 4 );
	REQUIRE_FALSE( bitset.none() );
	REQUIRE( bitset.any() );
	REQUIRE_FALSE( bitset.all() );

	bitset.fill();
	REQUIRE_FALSE( bitset.none() );
	REQUIRE( bitset.any() );
	REQUIRE( bitset.all() );
}

TEST_CASE( "BitSet equals and operators equality", "[util][math][bitset]" )
{
	BitSet a( 100 );
	BitSet b( 100 );
	BitSet c( 100 );
	BitSet d( 101 );

	a.set( 0 );
	a.set( 99 );

	b.set( 0 );
	b.set( 99 );

	c.set( 0 );

	REQUIRE( a.equals( b ) );
	REQUIRE( a == b );
	REQUIRE_FALSE( a != b );

	REQUIRE_FALSE( a.equals( c ) );
	REQUIRE_FALSE( a == c );
	REQUIRE( a != c );

	REQUIRE_FALSE( a.equals( d ) );
	REQUIRE_FALSE( a == d );
	REQUIRE( a != d );
}

TEST_CASE( "BitSet merge", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	const BitSet c = a.merge( b );

	REQUIRE( c.count() == 5 );
	REQUIRE( c.test( 1 ) );
	REQUIRE( c.test( 5 ) );
	REQUIRE( c.test( 9 ) );
	REQUIRE( c.test( 70 ) );
	REQUIRE( c.test( 99 ) );

	REQUIRE_FALSE( c.test( 0 ) );
	REQUIRE_FALSE( c.test( 2 ) );

	REQUIRE( a.count() == 3 );
	REQUIRE( b.count() == 4 );
}

TEST_CASE( "BitSet mergeInPlace", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	b.set( 5 );
	b.set( 9 );

	BitSet & ref = a.mergeInPlace( b );

	REQUIRE( &ref == &a );
	REQUIRE( a.count() == 3 );
	REQUIRE( a.test( 1 ) );
	REQUIRE( a.test( 5 ) );
	REQUIRE( a.test( 9 ) );
}

TEST_CASE( "BitSet intersect", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	const BitSet c = a.intersect( b );

	REQUIRE( c.count() == 2 );
	REQUIRE_FALSE( c.test( 1 ) );
	REQUIRE( c.test( 5 ) );
	REQUIRE( c.test( 70 ) );
	REQUIRE_FALSE( c.test( 9 ) );
	REQUIRE_FALSE( c.test( 99 ) );
}

TEST_CASE( "BitSet intersectInPlace", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	BitSet & ref = a.intersectInPlace( b );

	REQUIRE( &ref == &a );
	REQUIRE( a.count() == 2 );
	REQUIRE_FALSE( a.test( 1 ) );
	REQUIRE( a.test( 5 ) );
	REQUIRE( a.test( 70 ) );
	REQUIRE_FALSE( a.test( 9 ) );
	REQUIRE_FALSE( a.test( 99 ) );
}

TEST_CASE( "BitSet subtract", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	const BitSet c = a.subtract( b );

	REQUIRE( c.count() == 1 );
	REQUIRE( c.test( 1 ) );
	REQUIRE_FALSE( c.test( 5 ) );
	REQUIRE_FALSE( c.test( 70 ) );
	REQUIRE_FALSE( c.test( 9 ) );
	REQUIRE_FALSE( c.test( 99 ) );
}

TEST_CASE( "BitSet subtractInPlace", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	BitSet & ref = a.subtractInPlace( b );

	REQUIRE( &ref == &a );
	REQUIRE( a.count() == 1 );
	REQUIRE( a.test( 1 ) );
	REQUIRE_FALSE( a.test( 5 ) );
	REQUIRE_FALSE( a.test( 70 ) );
}

TEST_CASE( "BitSet xorWith", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	const BitSet c = a.xorWith( b );

	REQUIRE( c.count() == 3 );
	REQUIRE( c.test( 1 ) );
	REQUIRE( c.test( 9 ) );
	REQUIRE( c.test( 99 ) );
	REQUIRE_FALSE( c.test( 5 ) );
	REQUIRE_FALSE( c.test( 70 ) );
}

TEST_CASE( "BitSet xorInPlace", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	BitSet & ref = a.xorInPlace( b );

	REQUIRE( &ref == &a );
	REQUIRE( a.count() == 3 );
	REQUIRE( a.test( 1 ) );
	REQUIRE( a.test( 9 ) );
	REQUIRE( a.test( 99 ) );
	REQUIRE_FALSE( a.test( 5 ) );
	REQUIRE_FALSE( a.test( 70 ) );
}

TEST_CASE( "BitSet operators", "[util][math][bitset]" )
{
	BitSet a( 128 );
	BitSet b( 128 );

	a.set( 1 );
	a.set( 5 );
	a.set( 70 );

	b.set( 5 );
	b.set( 9 );
	b.set( 70 );
	b.set( 99 );

	const BitSet merge	   = a | b;
	const BitSet intersect = a & b;
	const BitSet subtract  = a - b;
	const BitSet xored	   = a ^ b;

	REQUIRE( merge.count() == 5 );
	REQUIRE( merge.test( 1 ) );
	REQUIRE( merge.test( 5 ) );
	REQUIRE( merge.test( 9 ) );
	REQUIRE( merge.test( 70 ) );
	REQUIRE( merge.test( 99 ) );

	REQUIRE( intersect.count() == 2 );
	REQUIRE( intersect.test( 5 ) );
	REQUIRE( intersect.test( 70 ) );

	REQUIRE( subtract.count() == 1 );
	REQUIRE( subtract.test( 1 ) );

	REQUIRE( xored.count() == 3 );
	REQUIRE( xored.test( 1 ) );
	REQUIRE( xored.test( 9 ) );
	REQUIRE( xored.test( 99 ) );
}

TEST_CASE( "BitSet inplace operators", "[util][math][bitset]" )
{
	SECTION( "operator|=" )
	{
		BitSet a( 128 );
		BitSet b( 128 );

		a.set( 1 );
		b.set( 2 );

		a |= b;

		REQUIRE( a.count() == 2 );
		REQUIRE( a.test( 1 ) );
		REQUIRE( a.test( 2 ) );
	}

	SECTION( "operator&=" )
	{
		BitSet a( 128 );
		BitSet b( 128 );

		a.set( 1 );
		a.set( 2 );
		b.set( 2 );
		b.set( 3 );

		a &= b;

		REQUIRE( a.count() == 1 );
		REQUIRE_FALSE( a.test( 1 ) );
		REQUIRE( a.test( 2 ) );
		REQUIRE_FALSE( a.test( 3 ) );
	}

	SECTION( "operator-=" )
	{
		BitSet a( 128 );
		BitSet b( 128 );

		a.set( 1 );
		a.set( 2 );
		b.set( 2 );

		a -= b;

		REQUIRE( a.count() == 1 );
		REQUIRE( a.test( 1 ) );
		REQUIRE_FALSE( a.test( 2 ) );
	}

	SECTION( "operator^=" )
	{
		BitSet a( 128 );
		BitSet b( 128 );

		a.set( 1 );
		a.set( 2 );
		b.set( 2 );
		b.set( 3 );

		a ^= b;

		REQUIRE( a.count() == 2 );
		REQUIRE( a.test( 1 ) );
		REQUIRE_FALSE( a.test( 2 ) );
		REQUIRE( a.test( 3 ) );
	}
}

TEST_CASE( "BitSet forEachSetBit iteration order", "[util][math][bitset]" )
{
	BitSet bitset( 130 );

	bitset.set( 0 );
	bitset.set( 2 );
	bitset.set( 63 );
	bitset.set( 64 );
	bitset.set( 65 );
	bitset.set( 129 );

	std::vector<size_t> values;
	bitset.forEachSetBit( [ & ]( const size_t p_index ) { values.push_back( p_index ); } );

	const std::vector<size_t> expected = { 0, 2, 63, 64, 65, 129 };
	REQUIRE( values == expected );
}

TEST_CASE( "BitSet forEachSetBit empty", "[util][math][bitset]" )
{
	const BitSet bitset( 128 );

	size_t count = 0;
	bitset.forEachSetBit( [ & ]( const size_t ) { ++count; } );

	REQUIRE( count == 0 );
}

TEST_CASE( "BitSet fill and subtract preserve tail mask", "[util][math][bitset]" )
{
	BitSet a( 100 );
	BitSet b( 100 );

	a.fill();
	b.fill();

	a.reset( 99 );
	b.reset( 98 );

	const BitSet c = a.subtract( b );

	REQUIRE( c.count() == 1 );
	REQUIRE( c.test( 98 ) );
	REQUIRE_FALSE( c.test( 99 ) );

	std::vector<size_t> values;
	c.forEachSetBit( [ & ]( const size_t p_index ) { values.push_back( p_index ); } );

	REQUIRE( values.size() == 1 );
	REQUIRE( values[ 0 ] == 98 );
}

TEST_CASE( "BitSet xor preserve tail mask", "[util][math][bitset]" )
{
	BitSet a( 100 );
	BitSet b( 100 );

	a.fill();
	b.fill();

	a.reset( 10 );
	b.reset( 20 );

	const BitSet c = a.xorWith( b );

	REQUIRE( c.count() == 2 );
	REQUIRE( c.test( 10 ) );
	REQUIRE( c.test( 20 ) );

	std::vector<size_t> values;
	c.forEachSetBit( [ & ]( const size_t p_index ) { values.push_back( p_index ); } );

	const std::vector<size_t> expected = { 10, 20 };
	REQUIRE( values == expected );
}

TEST_CASE( "BitSet data view reflects state", "[util][math][bitset]" )
{
	BitSet bitset( 130 );

	bitset.set( 0 );
	bitset.set( 63 );
	bitset.set( 64 );
	bitset.set( 129 );

	const std::vector<uint64_t> & data = bitset.data();

	REQUIRE( data.size() == 3 );
	REQUIRE( ( data[ 0 ] & ( 1ull << 0 ) ) != 0ull );
	REQUIRE( ( data[ 0 ] & ( 1ull << 63 ) ) != 0ull );
	REQUIRE( ( data[ 1 ] & ( 1ull << 0 ) ) != 0ull );
	REQUIRE( ( data[ 2 ] & ( 1ull << 1 ) ) != 0ull );
}
