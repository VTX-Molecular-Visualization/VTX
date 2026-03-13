#include <catch2/catch_test_macros.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[unit]" )
{
	VTX::LOGGER::init( VTX::Util::Filesystem::getExecutableDir() / "logs_tests", true );

	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
}
