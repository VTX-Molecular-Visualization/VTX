#include <app/fixture.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Tool::Example", "[tool.example]" )
{
	using namespace VTX;
	{
		{
			App::Fixture app;
		}
		{
			App::Fixture app;
		}
		{
			App::Fixture app;
		}
		{
			App::Fixture app;
		}
		{
			App::Fixture app;
		}
	}

	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
}

TEST_CASE( "Tool::Example2", "[tool.example]" )
{
	using namespace VTX;
	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
	{
		App::Fixture app;
	}
}
