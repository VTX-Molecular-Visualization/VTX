#include <app/fixture.hpp>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

int main( int argc, char * argv[] )
{
	std::cerr << "VTX_TEST_MAIN_BEGIN\n";
	const int result = Catch::Session().run( argc, argv );
	std::cerr << "VTX_TEST_MAIN_END=" << result << "\n";
	return result;
}

TEST_CASE( "Tool::Example", "[tool][example]" ) { CHECK( 1 + 1 == 2 ); }
