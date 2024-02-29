#include "util/app.hpp"
#include <app/core/uid/uid.hpp>
#include <app/core/uid/uid_registration.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <limits>
#include <util/exceptions.hpp>

TEST_CASE( "VTX_APP - UID", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App::Core::UID;

	VTX_INFO( "VTX_APP - UID" );

	uid		 value;
	UIDRange range;

	UIDRegistration registration = UIDRegistration();

	value = registration.registerValue();
	CHECK( value == 0 );

	value = registration.registerValue();
	CHECK( value == 1 );

	range = registration.registerRange( 50 );
	CHECK( range.getFirst() == 2 );
	CHECK( range.getCount() == 50 );

	registration.unregister( 1 );
	value = registration.registerValue();
	CHECK( value == 1 );

	value = registration.registerValue();
	CHECK( value == 52 );

	registration.unregister( range );
	value = registration.registerValue();
	CHECK( value == 2 );
	range = registration.registerRange( 50 );
	CHECK( range.getFirst() == 53 );
	range = registration.registerRange( 20 );
	CHECK( range.getFirst() == 3 );

	try
	{
		registration.registerRange( std::numeric_limits<uid>().max() );
	}
	catch ( const VTXException & p_e )
	{
	}
	catch ( const std::exception & p_e )
	{
		CHECK( false );
	}

	registration.clear();
}