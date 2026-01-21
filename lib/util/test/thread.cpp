#include <catch2/catch_test_macros.hpp>
#include <util/thread.hpp>
#include <vector>

TEST_CASE( "ThreadedVector", "[thread]" )
{
	using namespace VTX::Util;

	SECTION( "Size control and availability query" )
	{
		ThreadedVector<int> v0( 5 );
		CHECK_FALSE( v0.isAvailable( 0 ) );
		v0.addNewElement( 11 );
		CHECK( v0.isAvailable( 0 ) );
		CHECK_FALSE( v0.isAvailable( 1 ) );
		v0.addNewElement( 11 * 2 );
		v0.addNewElement( 11 * 3 );
		v0.addNewElement( 11 * 4 );
		v0.addNewElement( 11 * 5 );
		CHECK( v0.isAvailable( 1 ) );
		CHECK( v0.isAvailable( 2 ) );
		CHECK( v0.isAvailable( 3 ) );
		CHECK( v0.isAvailable( 4 ) );
		CHECK_FALSE( v0.isAvailable( 5 ) );
		CHECK_THROWS( v0.addNewElement( 11 * 6 ) );
	}
	SECTION( "Reference validity" )
	{
		ThreadedVector<int> v1( 5 );
		ThreadedVector<int> v0( 5 );
		ThreadedVector<int> v2( 5 );
		v0.addNewElement( 11 );
		int * ref0 = nullptr;
		v0.get( 0, ref0 );
		CHECK_NOFAIL( ref0 != nullptr );
		CHECK( *ref0 == 11 );
		// Some operation on the heap
		v1.addNewElement( 11 * 2 );
		v1.addNewElement( 11 * 3 );
		v2.addNewElement( 11 * 2 );
		v2.addNewElement( 11 * 3 );
		v0.addNewElement( 11 * 2 );
		v0.addNewElement( 11 * 3 );
		v0.addNewElement( 11 * 4 );
		v0.addNewElement( 11 * 5 );
		int * ref0_again = nullptr;
		v0.get( 0, ref0_again );
		CHECK( ref0_again == ref0 );
		CHECK( *ref0 == 11 );
	}
}
