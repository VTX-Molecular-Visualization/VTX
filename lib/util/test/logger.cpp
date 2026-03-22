#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[unit]" )
{
	VTX::LOGGER::init( VTX::Util::Filesystem::getExecutableDir() / "logs_tests", true );

	std::vector<VTX::Util::LogInfo> logs;
	const auto callbackId = VTX::LOGGER::onLog.add( [&]( const VTX::Util::LogInfo & p_log ) { logs.push_back( p_log ); } );

	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
	VTX::VTX_PYTHON_IN( "python in" );
	VTX::VTX_PYTHON_OUT( "python out" );

	VTX::LOGGER::flush();

	REQUIRE( logs.size() >= 4 );
	CHECK( logs[ logs.size() - 4 ].message == "info without args" );
	CHECK( logs[ logs.size() - 4 ].hint == VTX::Util::LOG_HINT::STD );
	CHECK( logs[ logs.size() - 3 ].message == "info with args: test 42" );
	CHECK( logs[ logs.size() - 3 ].hint == VTX::Util::LOG_HINT::STD );
	CHECK( logs[ logs.size() - 2 ].message == "python in" );
	CHECK( logs[ logs.size() - 2 ].hint == VTX::Util::LOG_HINT::PY_IN );
	CHECK( logs[ logs.size() - 1 ].message == "python out" );
	CHECK( logs[ logs.size() - 1 ].hint == VTX::Util::LOG_HINT::PY_OUT );

	VTX::LOGGER::onLog.remove( callbackId );
	VTX::LOGGER::stop();
}
