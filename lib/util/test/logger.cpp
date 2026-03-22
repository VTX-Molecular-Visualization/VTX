#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[unit]" )
{
	VTX::LOGGER::init( VTX::Util::Filesystem::getExecutableDir() / "logs_tests", false );

	struct LogState
	{
		std::mutex							   mutex;
		std::condition_variable				   cv;
		std::map<std::string, VTX::Util::LOG_HINT> received;
	};

	auto state = std::make_shared<LogState>();
	const auto callbackId = VTX::LOGGER::onLog.add( [ state ]( const VTX::Util::LogInfo & p_log ) {
		{
			const std::lock_guard<std::mutex> lock( state->mutex );
			state->received[ p_log.message ] = p_log.hint;
		}
		state->cv.notify_one();
	} );

	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
	VTX::VTX_PYTHON_IN( "python in" );
	VTX::VTX_PYTHON_OUT( "python out" );

	VTX::LOGGER::flush();

	{
		std::unique_lock<std::mutex> lock( state->mutex );
		REQUIRE( state->cv.wait_for( lock, std::chrono::seconds( 2 ), [ & ]() {
			return state->received.contains( "info without args" ) && state->received.contains( "info with args: test 42" )
				   && state->received.contains( "python in" ) && state->received.contains( "python out" );
		} ) );
		CHECK( state->received.at( "info without args" ) == VTX::Util::LOG_HINT::STD );
		CHECK( state->received.at( "info with args: test 42" ) == VTX::Util::LOG_HINT::STD );
		CHECK( state->received.at( "python in" ) == VTX::Util::LOG_HINT::PY_IN );
		CHECK( state->received.at( "python out" ) == VTX::Util::LOG_HINT::PY_OUT );
	}

	VTX::LOGGER::onLog.remove( callbackId );
	VTX::LOGGER::stop();
}
