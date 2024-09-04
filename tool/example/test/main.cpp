#include <app/application/scene.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Tool::Example", "[tool.example]" )
{
	VTX::APP::init();
	VTX::APP().start( { 0, nullptr } );
}
