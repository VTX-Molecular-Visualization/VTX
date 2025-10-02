#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>

int value = 0;

struct TestEvent
{
};

class ClassTest
{
  public:
	void memberFunction( TestEvent & p_e ) { value++; }
};

void freeFunction( TestEvent & p_e ) { value++; }

TEST_CASE( "VTX_UTIL - EVENT HUB", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	VTX_INFO( "VTX_APP - EVENT HUB" );

	EventHub hub;

	// Free function.
	auto conn = hub.connect<TestEvent, &freeFunction>();

	// Local method.
	ClassTest classTest;
	hub.connect<TestEvent, &ClassTest::memberFunction>( classTest );

	// Lambda not owned by hub.
	auto lambda = [ & ]( TestEvent & p_e ) { value++; };
	hub.connect<TestEvent>( lambda );

	CHECK( hub.ownedConnectionCount() == 0 );

	// Owned lambda.
	hub.connect<TestEvent>( [ & ]( TestEvent & p_e ) { value++; } );

	// Owning lambdas.
	CHECK( hub.ownedConnectionCount() == 1 );

	// Trigger event.
	TestEvent event;
	hub.trigger<TestEvent>( event );

	CHECK( value == 4 );

	hub.trigger<TestEvent>();

	// 5+4 (connectOnce disconnected).
	CHECK( value == 8 );

	hub.disconnect( conn );
	hub.trigger<TestEvent>();

	// 9+3 (free function disconnected).
	CHECK( value == 11 );

	hub.disconnectAllOf( classTest );
	hub.trigger<TestEvent>();

	// 12+2 (classTest disconnected).
	CHECK( value == 13 );

	hub.disconnectAllListenersOf<TestEvent>();
	hub.trigger<TestEvent>();

	// 14+0 (all disconnected).

	CHECK( value == 13 );

	hub.connect<TestEvent, &freeFunction>();
	hub.enqueue<TestEvent>( event );
	hub.enqueue<TestEvent>();

	// Nothing.
	CHECK( value == 13 );

	hub.update();

	// Equeued event processed.
	CHECK( value == 15 );

	{
		EventHub::ScopedConnection c = hub.connect<TestEvent, &ClassTest::memberFunction>( classTest );
		hub.trigger<TestEvent>();

		// 16+2.
		CHECK( value == 17 );
	}

	hub.trigger<TestEvent>();

	// 18+1 (classTest disconnected when out of scope).
	CHECK( value == 18 );

	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.trigger<TestEvent>();

	// Cant connect multiple times.
	CHECK( value == 19 );
}
