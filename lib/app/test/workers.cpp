#include "util/app.hpp"
#include <app/core/threading/base_thread.hpp>
#include <app/core/threading/threading_system.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>

namespace
{
	const int MANUALLY_STOPPED_THREAD = 666;
	const int FINISHED_THREAD		  = 100;
	const int THREAD_NUM_STEPS		  = 100;
} // namespace

TEST_CASE( "VTX_APP - Workers", "[integration][workers]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	App::Core::Threading::BaseThread::StoppableAsyncOp asyncOp
		= []( VTX::Util::StopToken p_token, App::Core::Threading::BaseThread & p_thread )
	{
		for ( int i = 0; i < THREAD_NUM_STEPS; i++ )
		{
			if ( p_token.stop_requested() )
			{
				p_thread.set<int>( MANUALLY_STOPPED_THREAD );
				goto endOfThread;
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
			const float progress = ( i + 1 ) / static_cast<float>( THREAD_NUM_STEPS );
			p_thread.setProgress( progress );
		}

		p_thread.set<int>( FINISHED_THREAD );
	endOfThread:
		return 1;
	};

	{
		App::Core::Threading::BaseThread & thread = THREADING_SYSTEM().createThread(
			asyncOp,
			[]( App::Core::Threading::BaseThread & p_thread, uint p_res )
			{
				const int threadData = p_thread.get<int>();
				CHECK( p_res == 1 );
				CHECK( threadData == FINISHED_THREAD );

				VTX_INFO( "AsyncOp finish with data {} and result {}", threadData, p_res );
			}
		);

		int	 onProgressCallNum	= 0;
		bool progressAsExpected = true;
		thread.onProgress += [ &onProgressCallNum, &progressAsExpected ]( const float p_progress ) mutable
		{
			onProgressCallNum++;
			const float expectedProgress = ( static_cast<float>( onProgressCallNum ) ) / 100.f;
			progressAsExpected &= expectedProgress == p_progress;
		};
		thread.wait();
		CHECK( progressAsExpected == true );
		CHECK( onProgressCallNum == 100 );
	}

	App::Core::Threading::BaseThread & threadToWait = THREADING_SYSTEM().createThread( asyncOp );
	CHECK( !threadToWait.isFinished() );

	Util::Chrono chrono = Util::Chrono();
	chrono.start();
	threadToWait.wait();
	chrono.stop();
	CHECK( threadToWait.isFinished() );
	CHECK( chrono.elapsedTime() > 1.3f ); // Ensure wait sufficient time (sleep_for not really accurate).

	App::Core::Threading::BaseThread & threadToStop = THREADING_SYSTEM().createThread( asyncOp );
	CHECK( !threadToStop.isFinished() );

	chrono.start();
	threadToStop.stop();
	threadToStop.wait();
	CHECK( threadToStop.isFinished() );
	chrono.stop();
	CHECK( threadToStop.get<int>() == MANUALLY_STOPPED_THREAD ); // Ensure thread stopped before it's real end.

	// Flush WorkerManager
	// APP::update( 0.f );
};
