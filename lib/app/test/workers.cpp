#include <app/fixture.hpp>
#include <app/services.hpp>
#include <app/threading/base_thread.hpp>
#include <app/threading/thread_manager.hpp>
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

	VTX::App::Threading::BaseThread::StoppableAsyncOp asyncOp
		= []( VTX::Util::StopToken p_token, VTX::App::Threading::BaseThread & p_thread ) -> VTX::uint
	{
		for ( int i = 0; i < THREAD_NUM_STEPS; i++ )
		{
			if ( p_token.stop_requested() )
			{
				p_thread.set<int>( MANUALLY_STOPPED_THREAD );
				return 1u;
			}
			std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
			const float progress = ( i + 1 ) / static_cast<float>( THREAD_NUM_STEPS );
			p_thread.setProgress( progress );
		}

		p_thread.set<int>( FINISHED_THREAD );
		return 1u;
	};

} // namespace

TEST_CASE( "VTX_APP - Workers - Wait", "[integration][workers][wait]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	App::Threading::BaseThread & threadToWait = THREAD().createThread( asyncOp );
	CHECK( !threadToWait.isFinished() );

	Util::Chrono chrono = Util::Chrono();
	chrono.start();
	threadToWait.wait();

	CHECK( threadToWait.isFinished() );
	CHECK( chrono.elapsedTime() > 1.3f ); // Ensure waited sufficient time (sleep_for not really accurate).
}

TEST_CASE( "VTX_APP - Workers - Stop", "[integration][workers][stop]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	App::Threading::BaseThread & threadToStop = THREAD().createThread( asyncOp );
	CHECK( !threadToStop.isFinished() );

	threadToStop.stop();
	threadToStop.wait();
	CHECK( threadToStop.isFinished() );
	CHECK( threadToStop.get<int>() == MANUALLY_STOPPED_THREAD );
}

TEST_CASE( "VTX_APP - Workers - Progress", "[integration][workers][progress]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	bool callbackCalled		= false;
	bool callbackStopped	= false;
	int	 callbackThreadData = 0;

	App::Threading::BaseThread & thread = THREAD().createThread(
		asyncOp,
		[ &callbackCalled, &callbackStopped, &callbackThreadData ](
			App::Threading::BaseThread & p_thread, uint, bool p_manuallyStopped )
		{
			callbackCalled		= true;
			callbackStopped		= p_manuallyStopped;
			callbackThreadData	= p_thread.get<int>();
		}
	);

	int	 onProgressCallNum	= 0;
	bool progressAsExpected = true;
	thread.onProgress += [ &onProgressCallNum, &progressAsExpected ]( const float p_progress ) mutable
	{
		onProgressCallNum++;
		const float expectedProgress = static_cast<float>( onProgressCallNum ) / 100.f;
		progressAsExpected &= expectedProgress == p_progress;
	};

	thread.wait();

	CHECK( callbackCalled == true );
	CHECK( callbackStopped == false );
	CHECK( callbackThreadData == FINISHED_THREAD );
	CHECK( progressAsExpected == true );
	CHECK( onProgressCallNum == THREAD_NUM_STEPS );
}
