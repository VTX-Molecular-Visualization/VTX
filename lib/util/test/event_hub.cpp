#include <atomic>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <util/event_hub.hpp>
#include <vector>

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
	const TestEvent event;
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
		const EventHub::ScopedConnection c = hub.connect<TestEvent, &ClassTest::memberFunction>( classTest );
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

TEST_CASE( "EventHub concurrent enqueue", "[unit]" )
{
	struct Event
	{
		int id;
	};

	VTX::Util::EventHub hub;
	constexpr int		producerCount	  = 4;
	constexpr int		eventsPerProducer = 256;
	std::vector<int>	received( producerCount * eventsPerProducer, 0 );
	const auto			mainThread	 = std::this_thread::get_id();
	bool				onMainThread = true;
	auto				listener	 = [ & ]( const Event & p_event )
	{
		onMainThread = onMainThread && std::this_thread::get_id() == mainThread;
		++received.at( p_event.id );
	};
	hub.connect<Event>( listener );
	std::atomic<bool>		  start		= false;
	std::atomic<int>		  completed = 0;
	std::vector<std::jthread> producers;
	for ( int producer = 0; producer < producerCount; ++producer )
	{
		producers.emplace_back(
			[ &, producer ]
			{
				while ( not start.load() )
				{
					std::this_thread::yield();
				}
				for ( int index = 0; index < eventsPerProducer; ++index )
				{
					hub.enqueue<Event>( producer * eventsPerProducer + index );
				}
				++completed;
			} );
	}
	start = true;
	while ( completed.load() != producerCount )
	{
		hub.update();
		std::this_thread::yield();
	}
	producers.clear();
	hub.update();
	CHECK( onMainThread );
	for ( const int count : received )
	{
		CHECK( count == 1 );
	}
}

TEST_CASE( "EventHub defers reentrant enqueue and clears by type", "[unit]" )
{
	struct Event
	{
	};

	struct OtherEvent
	{
	};

	VTX::Util::EventHub hub;
	int					received	  = 0;
	int					otherReceived = 0;
	auto				listener	  = [ & ]( const Event & )
	{
		++received;
		hub.enqueue<OtherEvent>();
	};
	auto otherListener = [ & ]( const OtherEvent & ) { ++otherReceived; };
	hub.connect<Event>( listener );
	hub.connect<OtherEvent>( otherListener );
	hub.enqueue<Event>();
	hub.update();
	CHECK( received == 1 );
	CHECK( otherReceived == 0 );
	hub.update();
	CHECK( otherReceived == 1 );
	hub.enqueue<Event>();
	hub.enqueue<OtherEvent>();
	hub.clear<Event>();
	hub.update();
	CHECK( received == 1 );
	CHECK( otherReceived == 2 );
}
