#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <util/action_manager.hpp>
#include <util/thread/base_thread.hpp>
#include <util/thread/thread_manager.hpp>

namespace
{
	using ActionManager = VTX::Util::ActionManager;
	using BaseThread	= VTX::Util::Thread::BaseThread;
	using ThreadData	= VTX::Util::Thread::ThreadData;
	using ThreadManager = VTX::Util::Thread::ThreadManager;

	struct InlineAction
	{
		void execute( std::atomic<int> & p_counter, const int p_value ) { p_counter += p_value; }
	};

	struct ThrowingAction
	{
		void execute() { throw std::runtime_error( "Expected action failure." ); }
	};

	struct ThreadedAction
	{
		ThreadedAction() = default;

		explicit ThreadedAction( ThreadData ) {}

		void execute( std::atomic<std::thread::id> * const p_threadId, std::atomic_bool * const p_done )
		{
			p_threadId->store( std::this_thread::get_id() );
			p_done->store( true );
		}
	};

	struct ThrowingThreadedAction
	{
		ThrowingThreadedAction() = default;

		explicit ThrowingThreadedAction( ThreadData ) {}

		void execute() { throw std::runtime_error( "Expected threaded action failure." ); }
	};

	bool waitFor( const std::atomic_bool & p_value )
	{
		const auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds( 1 );
		while ( not p_value.load() && std::chrono::steady_clock::now() < timeout )
		{
			std::this_thread::yield();
		}
		return p_value.load();
	}
} // namespace

static_assert( not VTX::Util::ThreadableAction<InlineAction> );
static_assert( not VTX::Util::ThreadableAction<ThrowingAction> );
static_assert( VTX::Util::ThreadableAction<ThreadedAction> );
static_assert( VTX::Util::ThreadableAction<ThrowingThreadedAction> );

TEST_CASE( "VTX_UTIL - ActionManager - inline action", "[unit]" )
{
	const ActionManager manager;
	std::atomic<int>	counter = 0;

	manager.execute<InlineAction>( counter, 2 );
	CHECK( counter.load() == 2 );

	InlineAction action;
	manager.execute( action, counter, 3 );
	CHECK( counter.load() == 5 );

	ActionManager::run( action, counter, 4 );
	CHECK( counter.load() == 9 );
}

TEST_CASE( "VTX_UTIL - ActionManager - exceptions", "[unit]" )
{
	const ActionManager manager;
	ThrowingAction		action;

	CHECK_NOTHROW( manager.execute<ThrowingAction>() );
	CHECK_NOTHROW( manager.execute( action ) );
	CHECK_THROWS_AS( ActionManager::run( action ), std::runtime_error );
	CHECK_NOTHROW( manager.execute<ThrowingThreadedAction>() );
}

TEST_CASE( "VTX_UTIL - ActionManager - threadable action without thread manager", "[unit]" )
{
	const ActionManager			 manager;
	std::atomic<std::thread::id> threadId;
	std::atomic_bool			 done = false;

	manager.execute<ThreadedAction>( &threadId, &done );

	CHECK( done.load() );
	CHECK( threadId.load() == std::this_thread::get_id() );
}

TEST_CASE( "VTX_UTIL - ActionManager - threadable action with thread manager", "[unit]" )
{
	ThreadManager				 threads;
	const ActionManager			 manager( threads );
	std::atomic<std::thread::id> threadId;
	std::atomic_bool			 done = false;

	manager.execute<ThreadedAction>( &threadId, &done );

	REQUIRE( waitFor( done ) );
	CHECK( threadId.load() != std::this_thread::get_id() );
}

TEST_CASE( "VTX_UTIL - ActionManager - threaded failure", "[unit]" )
{
	ThreadManager	 threads;
	std::atomic_bool terminated = false;
	std::atomic_bool failed		= false;
	threads.setDefaultTerminatedCallback(
		[ &terminated, &failed ]( const BaseThread & p_thread )
		{
			failed	   = p_thread.hasFailed();
			terminated = true;
		}
	);
	const ActionManager manager( threads );

	manager.execute<ThrowingThreadedAction>();

	const auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds( 1 );
	while ( not terminated.load() && std::chrono::steady_clock::now() < timeout )
	{
		threads.update();
		std::this_thread::yield();
	}

	REQUIRE( terminated.load() );
	CHECK( failed.load() );
}
