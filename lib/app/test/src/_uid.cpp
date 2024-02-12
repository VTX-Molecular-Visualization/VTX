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

	value = UIDRegistration::get().registerValue();
	CHECK( value == 0 );

	value = UIDRegistration::get().registerValue();
	CHECK( value == 1 );

	range = UIDRegistration::get().registerRange( 50 );
	CHECK( range.getFirst() == 2 );
	CHECK( range.getCount() == 50 );

	UIDRegistration::get().unregister( 1 );
	value = UIDRegistration::get().registerValue();
	CHECK( value == 1 );

	value = UIDRegistration::get().registerValue();
	CHECK( value == 52 );

	UIDRegistration::get().unregister( range );
	value = UIDRegistration::get().registerValue();
	CHECK( value == 2 );
	range = UIDRegistration::get().registerRange( 50 );
	CHECK( range.getFirst() == 53 );
	range = UIDRegistration::get().registerRange( 20 );
	CHECK( range.getFirst() == 3 );

	try
	{
		UIDRegistration::get().registerRange( std::numeric_limits<uid>().max() );
	}
	catch ( const VTXException & p_e )
	{
	}
	catch ( const std::exception & p_e )
	{
		CHECK( false );
	}

	UIDRegistration::get().clear();
}
