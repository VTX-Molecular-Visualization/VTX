#include "util/app.hpp"
#include <app/fixture.hpp>
#include <app/monitoring/constants.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <renderer/facade.hpp>
#include <thread>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/monitoring/stats.hpp>

void displaySimpleInfo()
{
	using namespace VTX;
	using namespace VTX::App;

	const Util::Monitoring::Stats & stats = STATS();

	const float tickrate   = stats.getAverage<float>( Monitoring::TICK_RATE_KEY );
	const uint	tickrateMs = uint( tickrate * 1000.f );
	const uint	fps		   = tickrate <= 0 ? 0 : ( 1000 / tickrateMs );
	const float renderTime = stats.getAverage<float>( Monitoring::RENDER_DURATION_KEY );

	VTX_INFO( "FPS: {} - Tickrate: {}ms - Render time: {}ms", fps, tickrateMs, renderTime );
}

void displayFullInfo()
{
	using namespace VTX;
	using namespace VTX::App;

	const Util::Monitoring::Stats & stats = STATS();

	const float tickrate   = stats.getAverage<float>( Monitoring::TICK_RATE_KEY );
	const uint	tickrateMs = uint( tickrate * 1000.f );
	const uint	fps		   = tickrate <= 0 ? 0 : ( 1000 / tickrateMs );

	VTX_INFO(
		"\nPreUpdate : {}s\n \
			Update : {}s\n \
			Late Update : {}s\n \
			Post Update : {}s\n \
			Pre Render: {}s\n \
			Render: {}s\n \
			Post Render: {}s\n \
			FPS: {} - Tickrate: {}ms",
		stats.getAverage<float>( Monitoring::PRE_UPDATE_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::UPDATE_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::LATE_UPDATE_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::POST_UPDATE_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::PRE_RENDER_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::RENDER_DURATION_KEY ),
		stats.getAverage<float>( Monitoring::POST_RENDER_DURATION_KEY ),
		fps,
		tickrateMs
	);
}

TEST_CASE( "VTX_APP - Monitoring", "[integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	App::Fixture app;

	// APP::onPreUpdate +=
	//	[]( float deltaTime ) { std::this_thread::sleep_for( std::chrono::milliseconds( 12 ) ); };
	APP::onUpdate +=
		[]( float deltaTime, float elapsedTime ) { std::this_thread::sleep_for( std::chrono::milliseconds( 12 ) ); };
	// APP::onLateUpdate +=
	//	[]( float deltaTime ) { std::this_thread::sleep_for( std::chrono::milliseconds( 18 ) ); };
	APP::onPostUpdate += []( float deltaTime ) { std::this_thread::sleep_for( std::chrono::nanoseconds( 500 ) ); };

	long long lastDisplayTimestamp = Util::Chrono::getTimestamp();

	for ( int i = 0; i < 200; i++ )
	{
		APP::update( 0, 0 );

		const long long currentTimestamp = Util::Chrono::getTimestamp();
		if ( ( currentTimestamp - lastDisplayTimestamp ) >= 1000 )
		{
			displayFullInfo();
			lastDisplayTimestamp = Util::Chrono::getTimestamp();
		}
	}
};
