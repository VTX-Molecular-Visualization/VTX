#include <app/action/application.hpp>
#include <app/action/scene.hpp>
#include <app/application/scene.hpp>
#include <app/entity/scene.hpp>
#include <app/filesystem.hpp>
#include <app/fixture.hpp>
#include <app/vtx_app.hpp>
#include <app/fixture.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Tool::Example", "[tool.example]" )
{
	using namespace VTX;
	{
		{
			App::Fixture app;
			App::SCENE().reset();
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
