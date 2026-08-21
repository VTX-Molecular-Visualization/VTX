#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <util/chrono.hpp>
#include <util/thread/base_thread.hpp>
#include <util/thread/stop_token.hpp>
#include <util/thread/thread_manager.hpp>
#include <util/types.hpp>

namespace
{
	using BaseThread = VTX::Util::Thread::BaseThread;
	using StopToken	 = VTX::Util::Thread::StopToken;

	const VTX::uint MANUALLY_STOPPED_THREAD = 666;
	const VTX::uint FINISHED_THREAD			= 100;
	const int		THREAD_NUM_STEPS		= 100;

	BaseThread::AsyncOp asyncOp = []( StopToken p_token, BaseThread & p_thread ) -> VTX::uint
	{
		for ( int i = 0; i < THREAD_NUM_STEPS; i++ )
		{
			if ( p_token.stop_requested() )
			{
				return MANUALLY_STOPPED_THREAD;
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
			const float progress = ( i + 1 ) / static_cast<float>( THREAD_NUM_STEPS );
			p_thread.setProgress( progress );
		}

		return FINISHED_THREAD;
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

	class DerivedThread : public VTX::Util::Thread::BaseThread
	{
	  public:
		DerivedThread( VTX::Util::Thread::ThreadManager & p_manager, std::atomic_bool & p_executed ) :
			BaseThread( p_manager ), _executed( p_executed )
		{
		}

		void start()
		{
			BaseThread::_start(
				[ this ]( StopToken, VTX::Util::Thread::BaseThread & )
				{
					_executed = true;
					return 0u;
				}
			);
		}

	  private:
		std::atomic_bool & _executed;
	};

	class FailingThread final : public VTX::Util::Thread::BaseThread
	{
	  public:
		FailingThread( VTX::Util::Thread::ThreadManager & p_manager, std::atomic_bool & p_destroyed ) :
			BaseThread( p_manager ), _destroyed( p_destroyed )
		{
		}

		~FailingThread() override { _destroyed = true; }

		void start() { throw std::runtime_error( "Expected start failure." ); }

	  private:
		std::atomic_bool & _destroyed;
	};

} // namespace

TEST_CASE( "VTX_UTIL - Threading - Wait", "[threading][wait]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;

	Util::Thread::BaseThread & threadToWait = threadManager.createThread( asyncOp );
	CHECK( not threadToWait.isFinished() );

	Util::Chrono chrono = Util::Chrono();
	chrono.start();
	threadToWait.wait();

	CHECK( threadToWait.isFinished() );
	CHECK( chrono.elapsedTime() > 1.3f ); // Ensure waited sufficient time (sleep_for not really accurate).
}

TEST_CASE( "VTX_UTIL - Threading - Stop", "[threading][stop]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;

	uint					   result		= 0;
	Util::Thread::BaseThread & threadToStop = threadManager.createThread(
		asyncOp, [ &result ]( Util::Thread::BaseThread &, const uint p_result, const bool ) { result = p_result; }
	);
	CHECK( not threadToStop.isFinished() );

	threadToStop.stop();
	threadToStop.wait();
	CHECK( threadToStop.isFinished() );
	CHECK( result == MANUALLY_STOPPED_THREAD );
}

TEST_CASE( "VTX_UTIL - Threading - Synchronize", "[threading][synchronize]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	bool						ownerVoidResult = false;
	threadManager.synchronize( [ &ownerVoidResult ] { ownerVoidResult = true; } );
	CHECK( ownerVoidResult );
	CHECK( threadManager.synchronize( [] { return 21; } ) == 21 );

	int				 result			  = 0;
	bool			 workerVoidResult = false;
	bool			 exceptionCaught  = false;
	std::atomic_bool completed		  = false;
	threadManager.createThread(
		[ &result, &workerVoidResult, &exceptionCaught, &completed, &threadManager ](
			Util::Thread::StopToken, Util::Thread::BaseThread &
		)
		{
			threadManager.synchronize( [ &workerVoidResult ] { workerVoidResult = true; } );
			result = threadManager.synchronize( [] { return 42; } );
			try
			{
				threadManager.synchronize( [] { throw std::runtime_error( "Expected exception." ); } );
			}
			catch ( const std::runtime_error & )
			{
				exceptionCaught = true;
			}
			completed = true;
			return 0u;
		}
	);

	while ( not completed )
	{
		threadManager.update();
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}
	CHECK( result == 42 );
	CHECK( workerVoidResult );
	CHECK( exceptionCaught );
}

TEST_CASE( "VTX_UTIL - Threading - Cancel synchronization", "[threading][synchronize]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;

	std::atomic_bool		   waiting	 = false;
	bool					   cancelled = false;
	Util::Thread::BaseThread & thread	 = threadManager.createThread(
		[ &waiting, &cancelled, &threadManager ]( Util::Thread::StopToken, Util::Thread::BaseThread & )
		{
			waiting = true;
			try
			{
				threadManager.synchronize( [] { return 42; } );
			}
			catch ( const std::future_error & )
			{
				cancelled = true;
			}
			return 0u;
		}
	);

	while ( not waiting )
	{
		std::this_thread::yield();
	}
	thread.stop();
	thread.wait();

	CHECK( cancelled );
}

TEST_CASE( "VTX_UTIL - Threading - Dispatch", "[threading][dispatch]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;

	bool dispatched = false;
	threadManager.dispatch( [ &dispatched ] { dispatched = true; } );
	CHECK_FALSE( dispatched );
	threadManager.update();
	CHECK( dispatched );

	int						   workerResult = 0;
	std::atomic_bool		   workerQueued = false;
	Util::Thread::BaseThread & thread		= threadManager.createThread(
		[ &threadManager, &workerResult, &workerQueued ]( Util::Thread::StopToken, Util::Thread::BaseThread & )
		{
			threadManager.dispatch( [ &workerResult ] { workerResult = 42; } );
			workerQueued = true;
			return 0u;
		}
	);
	while ( not workerQueued )
	{
		std::this_thread::yield();
	}
	thread.wait();
	CHECK( workerResult == 0 );
	threadManager.update();
	CHECK( workerResult == 42 );

	threadManager.dispatch( [] { throw std::runtime_error( "Expected exception." ); } );
	CHECK_NOTHROW( threadManager.update() );
}

TEST_CASE( "VTX_UTIL - Threading - Progress", "[threading][progress]" )
{
	using namespace VTX;

	bool						 callbackCalled		= false;
	bool						 callbackStopped	= false;
	uint						 callbackThreadData = 0;
	int							 onProgressCallNum	= 0;
	float						 lastProgress		= 0.f;
	bool						 terminated			= false;
	Util::Thread::BaseThread::ID threadId {};

	Util::Thread::ThreadManager threadManager;
	threadManager.setDefaultProgressCallback(
		[ & ]( const Util::Thread::BaseThread & p_thread )
		{
			if ( p_thread.getId() == threadId )
			{
				onProgressCallNum++;
				lastProgress = p_thread.getProgress();
			}
		}
	);
	threadManager.setDefaultTerminatedCallback(
		[ & ]( const Util::Thread::BaseThread & p_thread )
		{
			if ( p_thread.getId() == threadId )
			{
				terminated = true;
			}
		}
	);

	Util::Thread::BaseThread & thread = threadManager.createThread(
		asyncOp,
		[ &callbackCalled,
		  &callbackStopped,
		  &callbackThreadData ]( Util::Thread::BaseThread &, const uint p_result, const bool p_manuallyStopped )
		{
			callbackCalled	   = true;
			callbackStopped	   = p_manuallyStopped;
			callbackThreadData = p_result;
		}
	);
	threadId = thread.getId();

	while ( not terminated )
	{
		threadManager.update();
		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
	}

	CHECK( callbackCalled == true );
	CHECK( callbackStopped == false );
	CHECK( callbackThreadData == FINISHED_THREAD );
	CHECK( onProgressCallNum > 0 );
	CHECK( lastProgress == 1.f );
}

TEST_CASE( "VTX_UTIL - Threading - Non-stoppable operation", "[threading][operation]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	std::atomic_bool			executed = false;
	Util::Thread::BaseThread &	thread	 = threadManager.createThread(
		[ &executed ]( Util::Thread::StopToken, Util::Thread::BaseThread & )
		{
			executed = true;
			return 0u;
		}
	);

	thread.wait();

	CHECK( executed.load() );
	CHECK( thread.isFinished() );
}

TEST_CASE( "VTX_UTIL - Threading - Manager lookup and stop", "[threading][stop]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	std::atomic_bool			started			   = false;
	bool						endManuallyStopped = false;
	bool						terminatedStopped  = false;
	threadManager.setDefaultTerminatedCallback( [ &terminatedStopped ]( const Util::Thread::BaseThread & p_thread )
												{ terminatedStopped = p_thread.isManuallyStopped(); } );

	Util::Thread::BaseThread & thread = threadManager.createThread(
		[ &started ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & )
		{
			started = true;
			while ( not p_stopToken.stop_requested() )
			{
				std::this_thread::yield();
			}
			return 0u;
		},
		[ &endManuallyStopped ]( Util::Thread::BaseThread &, const uint, const bool p_manuallyStopped )
		{ endManuallyStopped = p_manuallyStopped; }
	);

	REQUIRE( waitFor( started ) );
	const Util::Thread::BaseThread::ID id		   = thread.getId();
	Util::Thread::BaseThread *		   foundThread = threadManager.get( id );
	CHECK( foundThread == &thread );

	Util::Thread::BaseThread * missingThread = threadManager.get( Util::Thread::BaseThread::ID {} );
	CHECK( missingThread == nullptr );

	threadManager.stop( id );
	thread.wait();
	CHECK( thread.isManuallyStopped() );
	CHECK( endManuallyStopped );
	threadManager.update();
	CHECK( terminatedStopped );
}

TEST_CASE( "VTX_UTIL - Threading - Manager destruction", "[threading][lifecycle]" )
{
	using namespace VTX;

	SECTION( "Active thread" )
	{
		std::atomic_bool started = false;
		std::atomic_bool stopped = false;
		{
			Util::Thread::ThreadManager threadManager;
			threadManager.createThread(
				[ &started, &stopped ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & )
				{
					started = true;
					while ( not p_stopToken.stop_requested() )
					{
						std::this_thread::yield();
					}
					stopped = true;
					return 0u;
				}
			);
			REQUIRE( waitFor( started ) );
		}
		CHECK( stopped.load() );
	}

	SECTION( "Synchronized call" )
	{
		std::atomic_bool waiting   = false;
		bool			 cancelled = false;
		{
			Util::Thread::ThreadManager threadManager;
			threadManager.createThread(
				[ &waiting, &cancelled, &threadManager ]( Util::Thread::StopToken, Util::Thread::BaseThread & )
				{
					waiting = true;
					try
					{
						threadManager.synchronize( [] {} );
					}
					catch ( const std::future_error & )
					{
						cancelled = true;
					}
					return 0u;
				}
			);
			REQUIRE( waitFor( waiting ) );
		}
		CHECK( cancelled );
	}
}

TEST_CASE( "VTX_UTIL - Threading - Thread state", "[threading][state]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	std::atomic_bool			release = false;
	Util::Thread::BaseThread &	thread	= threadManager.createThread(
		[ &release ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & )
		{
			while ( not release && not p_stopToken.stop_requested() )
			{
				std::this_thread::yield();
			}
			return 0u;
		}
	);

	thread.setProgress( -1.f );
	CHECK( thread.getProgress() == 0.f );
	thread.setProgress( 2.f );
	CHECK( thread.getProgress() == 1.f );
	thread.setProgressText( "Working" );
	CHECK( thread.getProgressText() == "Working" );
	release = true;
	thread.wait();
}

TEST_CASE( "VTX_UTIL - Threading - Disabled callbacks", "[threading][callbacks]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	bool						progressCalled	 = false;
	bool						terminatedCalled = false;
	std::atomic_bool			release			 = false;
	threadManager.setDefaultProgressCallback( [ &progressCalled ]( const Util::Thread::BaseThread & )
											  { progressCalled = true; } );
	threadManager.setDefaultTerminatedCallback( [ &terminatedCalled ]( const Util::Thread::BaseThread & )
												{ terminatedCalled = true; } );
	Util::Thread::BaseThread & thread = threadManager.createThread(
		[ &release ]( Util::Thread::StopToken p_stopToken, Util::Thread::BaseThread & )
		{
			while ( not release && not p_stopToken.stop_requested() )
			{
				std::this_thread::yield();
			}
			return 0u;
		}
	);
	thread.setProgressCallback( {} );
	thread.setTerminatedCallback( {} );
	release = true;
	thread.wait();
	threadManager.update();

	CHECK( not progressCalled );
	CHECK( not terminatedCalled );
}

TEST_CASE( "VTX_UTIL - Threading - Derived thread", "[threading][derived]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager			 threadManager;
	std::atomic_bool					 executed = false;
	const std::shared_ptr<DerivedThread> thread	  = threadManager.createThread<DerivedThread>( executed );
	thread->wait();

	CHECK( executed.load() );
	CHECK( thread->isFinished() );
}

TEST_CASE( "VTX_UTIL - Threading - Start failure", "[threading][failure]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	std::atomic_bool			destroyed = false;

	CHECK_THROWS_AS( threadManager.createThread<FailingThread>( destroyed ), std::runtime_error );
	CHECK( destroyed.load() );
}

TEST_CASE( "VTX_UTIL - Threading - Callback failure", "[threading][failure]" )
{
	using namespace VTX;
	Util::Thread::ThreadManager threadManager;
	bool						progressCalled	 = false;
	bool						terminatedCalled = false;

	threadManager.setDefaultProgressCallback(
		[ &progressCalled ]( const Util::Thread::BaseThread & )
		{
			progressCalled = true;
			throw std::runtime_error( "Expected progress callback failure." );
		}
	);
	threadManager.setDefaultTerminatedCallback(
		[ &terminatedCalled ]( const Util::Thread::BaseThread & )
		{
			terminatedCalled = true;
			throw std::runtime_error( "Expected terminated callback failure." );
		}
	);

	Util::Thread::BaseThread & thread
		= threadManager.createThread( []( Util::Thread::StopToken, Util::Thread::BaseThread & ) { return 0u; } );
	const Util::Thread::BaseThread::ID threadId = thread.getId();
	thread.wait();

	CHECK_NOTHROW( threadManager.update() );
	CHECK( progressCalled );
	CHECK( terminatedCalled );
	CHECK( threadManager.get( threadId ) == nullptr );
}
