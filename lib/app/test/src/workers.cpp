#include <app/core/worker/base_thread.hpp>
#include <app/core/worker/worker_manager.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_APP - Workers", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Core::Worker::WorkerManager workerManager = Core::Worker::WorkerManager();

	Core::Worker::BaseThread & thread = workerManager.createThread(
		[]( Core::Worker::BaseThread & p_thread )
		{
			for ( int i = 0; i < 100; i++ )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 15 ) );
				p_thread.setProgress( ( i + 1 ) / 100.f );
			}

			p_thread.setData<int>( 100 );

			return 1;
		},
		[]( Core::Worker::BaseThread & p_thread, uint p_res )
		{
			const int threadData = p_thread.getData<int>();
			CHECK( p_res == 1 );
			CHECK( threadData == 100 );

			VTX_INFO( "AsyncOp finish with data {} and result {}", threadData, p_res );
		}
	);

	float lastProgress = 0.f;

	thread.progressCallback.addCallback(
		&thread,
		[ lastProgress ]( float p_progress ) mutable
		{
			CHECK( lastProgress < p_progress );
			lastProgress = p_progress;
		}
	);

	Core::Worker::BaseThread::AsyncOp waitTwoSecondsOp = []( Core::Worker::BaseThread & p_thread )
	{
		std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		return 0;
	};

	Core::Worker::BaseThread & threadToWait = workerManager.createThread( waitTwoSecondsOp );

	Util::Chrono chrono = Util::Chrono();
	chrono.start();
	threadToWait.wait();
	chrono.stop();
	CHECK( chrono.elapsedTime() > 1.8f ); // Ensure wait sufficient time (sleep_for not really accurate).

	Core::Worker::BaseThread & threadToStop = workerManager.createThread( waitTwoSecondsOp );

	chrono.start();
	threadToStop.stop();
	chrono.stop();
	CHECK( chrono.elapsedTime() < 0.2f ); // Ensure thread stopped before it's real end.
};
