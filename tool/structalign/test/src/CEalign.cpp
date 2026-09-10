#include <app/action/action_manager.hpp>
#include <app/fixture.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <app/trajectory/types.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <tool/structalign/backend/align.hpp>
#include <util/math.hpp>

struct Structure
{
	const VTX::Core::Struct::Topology *	  topology	 = nullptr;
	const VTX::Core::Struct::Trajectory * trajectory = nullptr;

	operator VTX::Tool::Structalign::backend::Structure() const

	{
		assert( topology != nullptr );
		assert( trajectory != nullptr );
		return VTX::Tool::Structalign::backend::Structure { *topology, trajectory->frames.back() };
	}
};

const float RSMD_TOLERANCE = 2.f; // idk which value to take

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

	Structure mobileStruct, staticStruct, controlStruct;
	for ( auto it_entity :
		  VTX::App::REG().group<VTX::IO::Metadata, VTX::Core::Struct::Topology, VTX::Core::Struct::Trajectory>() )
	{
		auto [ metadata, topology, trajectory ]
			= VTX::App::REG().get<VTX::IO::Metadata, VTX::Core::Struct::Topology, VTX::Core::Struct::Trajectory>(
				it_entity
			);
		if ( metadata.pdbIDCode == "1NAV" )
		{
			mobileStruct.topology	= &topology;
			mobileStruct.trajectory = &trajectory;
		}
		else if ( metadata.pdbIDCode == "8RQO" )
		{
			staticStruct.topology	= &topology;
			staticStruct.trajectory = &trajectory;
		}
		else
		{
			controlStruct.topology	 = &topology;
			controlStruct.trajectory = &trajectory;
		}
	}
	REQUIRE( mobileStruct.topology != nullptr );
	REQUIRE( mobileStruct.trajectory != nullptr );
	REQUIRE( staticStruct.topology != nullptr );
	REQUIRE( staticStruct.trajectory != nullptr );
	REQUIRE( controlStruct.topology != nullptr );
	REQUIRE( controlStruct.trajectory != nullptr );

	VTX::Tool::Structalign::backend::CEAlignParameters params { .staticStructure = staticStruct,
																.mobileStructure = mobileStruct };
	VTX::Tool::Structalign::backend::CEAlignResults	   results;
	VTX::Tool::Structalign::backend::align( params, results );

	float controlVsMobileRmsd = VTX::Util::Math::computeRmsd(
		mobileStruct.trajectory->frames.back(),
		controlStruct.trajectory->frames.back(),
		results.transformMatrix,
		VTX::MAT3F_ID
	);
	CHECK( controlVsMobileRmsd < RSMD_TOLERANCE );
}
