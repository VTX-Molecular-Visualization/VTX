#include <app/action/action_manager.hpp>
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <app/system/trajectory.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "structalign - CEAlign", "[align][CEalign][1nav_over_8rqo]" )
{
	VTX::App::Fixture f;
	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
		VTX::App::SESSION().getDataDir() / "cealign" / "8rqo.bcif.gz"
	);
	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
		VTX::App::SESSION().getDataDir() / "cealign" / "1NAV.pdb"
	);
	VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
		VTX::App::SESSION().getDataDir() / "cealign" / "1nav_over_8rqo.pdb"
	);
}
