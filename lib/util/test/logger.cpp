#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <vector>

// logger.hpp
TEST_CASE( "Util::Logger", "[unit]" )
{
	VTX::LOGGER::init( VTX::Util::Filesystem::getExecutableDir() / "logs_tests", true );

	std::vector<VTX::Util::LogInfo> logs;
	std::mutex						logsMutex;
	std::condition_variable			logsCv;
	const auto						callbackId = VTX::LOGGER::onLog.add(
		 [ & ]( const VTX::Util::LogInfo & p_log )
		 {
			 {
				 const std::lock_guard<std::mutex> lock( logsMutex );
				 logs.push_back( p_log );
			 }
			 logsCv.notify_one();
		 }
	 );

	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
	VTX::VTX_PYTHON_IN( "python in" );
	VTX::VTX_PYTHON_OUT( "python out" );

	VTX::LOGGER::flush();

	{
		std::unique_lock<std::mutex> lock( logsMutex );
		REQUIRE( logsCv.wait_for( lock, std::chrono::seconds( 2 ), [ & ]() { return logs.size() >= 4; } ) );
		CHECK( logs[ logs.size() - 4 ].message == "info without args" );
		CHECK( logs[ logs.size() - 4 ].hint == VTX::Util::LOG_HINT::STD );
		CHECK( logs[ logs.size() - 3 ].message == "info with args: test 42" );
		CHECK( logs[ logs.size() - 3 ].hint == VTX::Util::LOG_HINT::STD );
		CHECK( logs[ logs.size() - 2 ].message == "python in" );
		CHECK( logs[ logs.size() - 2 ].hint == VTX::Util::LOG_HINT::PY_IN );
		CHECK( logs[ logs.size() - 1 ].message == "python out" );
		CHECK( logs[ logs.size() - 1 ].hint == VTX::Util::LOG_HINT::PY_OUT );
	}

	VTX::LOGGER::onLog.remove( callbackId );
	VTX::LOGGER::stop();
}
