#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/event_hub.hpp>

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

	EventHub hub;

	// Free function.
	auto conn = hub.connect<TestEvent, &freeFunction>();

	// Local method.
	ClassTest classTest;
	hub.connect<TestEvent, &ClassTest::memberFunction>( classTest );

	// Lambda not owned by hub.
	auto lambda = [ & ]( TestEvent & p_e ) { value++; };
	hub.connect<TestEvent>( lambda );

	// Trigger event.
	TestEvent event;
	hub.trigger<TestEvent>( event );

	CHECK( value == 3 );

	hub.trigger<TestEvent>();

	CHECK( value == 6 );

	hub.disconnect( conn );
	hub.trigger<TestEvent>();

	CHECK( value == 8 );

	hub.disconnectAllOf( classTest );
	hub.trigger<TestEvent>();

	CHECK( value == 9 );

	hub.disconnectAllListenersOf<TestEvent>();
	hub.trigger<TestEvent>();

	CHECK( value == 9 );

	hub.connect<TestEvent, &freeFunction>();
	hub.enqueue<TestEvent>( event );
	hub.enqueue<TestEvent>();

	// Nothing.
	CHECK( value == 9 );

	hub.update();

	// Equeued event processed.
	CHECK( value == 11 );

	{
		EventHub::ScopedConnection c = hub.connect<TestEvent, &ClassTest::memberFunction>( classTest );
		hub.trigger<TestEvent>();

		CHECK( value == 13 );
	}

	hub.trigger<TestEvent>();

	// 18+1 (classTest disconnected when out of scope).
	CHECK( value == 14 );

	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.connect<TestEvent, &freeFunction>();
	hub.trigger<TestEvent>();

	// Cant connect multiple times.
	CHECK( value == 15 );
}
