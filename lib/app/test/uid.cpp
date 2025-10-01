#include "util/app.hpp"
#include <app/uid/uid.hpp>
#include <app/uid/uid_manager.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <exception>
#include <limits>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_APP - UID", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App::Uid;

	VTX_INFO( "VTX_APP - UID" );

	uid		 value;
	UIDRange range;

	UIDManager registration = UIDManager();

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

	try
	{
		registration.registerRange( std::numeric_limits<uid>().max() );
	}
	catch ( const VTXException & )
	{
	}
	catch ( const std::exception & p_e )
	{
		VTX::VTX_ERROR( "Unhandled exception : {}", p_e.what() );
		CHECK( false );
	}

	registration.clear();
}
