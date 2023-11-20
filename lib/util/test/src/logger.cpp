#include <catch2/catch_test_macros.hpp>
#include <util/logger.hpp>

// logger.hpp
TEST_CASE( "Util::Logger", "[unit]" )
{
	VTX::VTX_INFO( "info without args" );
	VTX::VTX_INFO( "info with args: {} {}", "test", 42 );
}
