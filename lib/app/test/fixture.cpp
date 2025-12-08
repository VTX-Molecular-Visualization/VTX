#include <app/fixture.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "FIXTURE", "[1]" )
{
	using namespace VTX;
	App::Fixture app;
}

TEST_CASE( "-FIXTURE", "[1]" )
{
	using namespace VTX;
	App::Fixture app;
}

TEST_CASE( "--FIXTURE", "[2]" )
{
	using namespace VTX;
	App::Fixture app;
}
TEST_CASE( "---FIXTURE", "[3]" )
{
	using namespace VTX;
	App::Fixture app;
}
