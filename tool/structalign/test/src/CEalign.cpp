#include <app/fixture.hpp>
#include <app/helper/scene.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <app/trajectory/types.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <tool/structalign/backend/align.hpp>
#include <util/action_manager.hpp>
#include <util/math.hpp>

namespace
{

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

	Structure getStructureByFileName( const std::string_view & p_fileName )
	{
		VTX::Entity system = VTX::App::Helper::Scene::getSystemByFileName( p_fileName );
		REQUIRE( system != VTX::InvalidEntity );
		auto [ topology, trajectory ]
			= VTX::App::REG().get<VTX::Core::Struct::Topology, VTX::Core::Struct::Trajectory>( system );
		return Structure { .topology = &topology, .trajectory = &trajectory };
	}

	struct AlignmentTestReport
	{
		bool rmsdUnderTolerance = false;
	};

	struct AlignementTestParameters
	{
		std::string_view staticStructFileName;
		std::string_view mobileStructFileName;
		std::string_view alignedMobileStructFileName;

		float rsmdTolerance = 2.f;
	};

	void testAlignment( const AlignementTestParameters & p_param, AlignmentTestReport & p_report )
	{
		AlignmentTestReport report;

		VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
			VTX::App::SESSION().getDataDir() / "cealign" / p_param.mobileStructFileName
		);
		VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
			VTX::App::SESSION().getDataDir() / "cealign" / p_param.staticStructFileName
		);
		VTX::App::ACTION().execute<VTX::App::Action::IO::LoadSystem>(
			VTX::App::SESSION().getDataDir() / "cealign" / p_param.alignedMobileStructFileName
		);

		Structure mobileStruct	= getStructureByFileName( p_param.mobileStructFileName ),
				  staticStruct	= getStructureByFileName( p_param.staticStructFileName ),
				  controlStruct = getStructureByFileName( p_param.alignedMobileStructFileName );

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
		p_report.rmsdUnderTolerance = controlVsMobileRmsd < p_param.rsmdTolerance;
	}

} // namespace

TEST_CASE( "structalign - CEAlign - 1nav_over_8rqo", "[align][CEalign][1nav_over_8rqo]" )
{
	VTX::App::Fixture		 f;
	AlignementTestParameters params { .staticStructFileName		   = "8rqo.bcif.gz",
									  .mobileStructFileName		   = "1NAV.pdb",
									  .alignedMobileStructFileName = "1nav_over_8rqo.pdb" };
	AlignmentTestReport		 report;
	testAlignment( params, report );
	CHECK( report.rmsdUnderTolerance );
}

TEST_CASE( "structalign - CEAlign - 5j6s_over_9td3", "[align][CEalign][5j6s_over_9td3]" )
{
	VTX::App::Fixture		 f;
	AlignementTestParameters params { .staticStructFileName		   = "9td3.bcif.gz",
									  .mobileStructFileName		   = "5j6s.bcif.gz",
									  .alignedMobileStructFileName = "5j6s_over_9td3.pdb" };
	AlignmentTestReport		 report;
	testAlignment( params, report );
	CHECK( report.rmsdUnderTolerance );
}

TEST_CASE( "structalign - CEAlign - 8fna_over_1nav", "[align][CEalign][8fna_over_1nav]" )
{
	VTX::App::Fixture		 f;
	AlignementTestParameters params { .staticStructFileName		   = "1NAV.pdb",
									  .mobileStructFileName		   = "8FNA.pdb",
									  .alignedMobileStructFileName = "8fna_over_1nav.pdb" };
	AlignmentTestReport		 report;
	testAlignment( params, report );
	CHECK( report.rmsdUnderTolerance );
}

TEST_CASE( "structalign - CEAlign - 8rqo_over_1nav", "[align][CEalign][8rqo_over_1nav]" )
{
	VTX::App::Fixture		 f;
	AlignementTestParameters params { .staticStructFileName		   = "1NAV.pdb",
									  .mobileStructFileName		   = "8rqo.bcif.gz",
									  .alignedMobileStructFileName = "8rqo_over_1nav.pdb" };
	AlignmentTestReport		 report;
	testAlignment( params, report );
	CHECK( report.rmsdUnderTolerance );
}
