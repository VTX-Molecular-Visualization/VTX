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
#include <util/types.hpp>

TEST_CASE( "VTX_APP - Workers", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	App::Core::Threading::BaseThread::AsyncOp asyncOp = []( App::Core::Threading::BaseThread & p_thread )
	{
		for ( int i = 0; i < 100; i++ )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
			const float progress = ( i + 1 ) / 100.f;
			p_thread.setProgress( progress );
		}

		p_thread.setData<int>( 100 );

		return 1;
	};

	App::Core::Threading::BaseThread & thread = THREADING_SYSTEM().createThread(
		asyncOp,
		[]( App::Core::Threading::BaseThread & p_thread, uint p_res )
		{
			const int threadData = p_thread.getData<int>();
			CHECK( p_res == 1 );
			CHECK( threadData == 100 );

			VTX_INFO( "AsyncOp finish with data {} and result {}", threadData, p_res );
		}
	);

	float lastProgress = 0.f;

	thread.onProgress += [ lastProgress ]( const float p_progress ) mutable
	{
		CHECK( lastProgress < p_progress );
		lastProgress = p_progress;
	};

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
	CHECK( threadToStop.isFinished() );
	threadToStop.wait();
	chrono.stop();
	CHECK( chrono.elapsedTime() < 0.2f ); // Ensure thread stopped before it's real end.

	// Flush WorkerManager
	// APP::update( 0.f );
};
