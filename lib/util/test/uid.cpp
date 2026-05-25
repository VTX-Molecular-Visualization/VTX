#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <limits>
#include <util/exceptions.hpp>
#include <util/uid.hpp>

TEST_CASE( "VTX_APP - UID", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;
	using namespace VTX::Util::Uid;

	using uid	   = uint32_t;
	using UIDRange = Math::Range<uid>;

	uid		 value;
	UIDRange range;

	Pool<uid> registration;

	value = registration.registerValue();
	CHECK( value == 1 );

	value = registration.registerValue();
	CHECK( value == 2 );

	range = registration.registerRange( 50 );
	CHECK( range.getFirst() == 3 );
	CHECK( range.getCount() == 50 );

	registration.unregister( 2 );
	value = registration.registerValue();
	CHECK( value == 2 );

	value = registration.registerValue();
	CHECK( value == 53 );

	registration.unregister( range );
	value = registration.registerValue();
	CHECK( value == 3 );
	range = registration.registerRange( 50 );
	CHECK( range.getFirst() == 54 );
	range = registration.registerRange( 20 );
	CHECK( range.getFirst() == 4 );

	CHECK_THROWS( registration.registerRange( TypeMax<uid> ) );

	registration.clear();
}
