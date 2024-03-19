#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
//
#include <tools/mdprep/gromacs/grompp.hpp>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>
#include <tools/mdprep/gromacs/util.hpp>
//
#include <mdprep/test/fixture.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - grompp - prepareJob", "[prepareJob][grompp]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	GromppInstructions in;
	in.min.emtol  = 666.6;
	in.min.emstep = 0.12;
	in.min.nsteps = 6028;
	in.fileStem	  = "1ubq";
	VTX::test::PrepareJobSetup		 f( "grompp", "5_grompp" );
	std::vector<const std::string *> previousOutputs;
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	CHECK( in.outputMdp.empty() == false );
	CHECK( in.outputMdp.string().ends_with( in.fileStem + ".mdp" ) == true );
	CHECK( in.outputTpr.empty() == false );
	CHECK( in.outputTpr.string().ends_with( in.fileStem + ".tpr" ) == true );
	CHECK( in.outputTop.empty() == false );
	CHECK( in.outputTop.string().ends_with( in.fileStem + ".top" ) == true );

	REQUIRE( in.inputMdp.empty() == false );
	CHECK( fs::exists( in.inputMdp ) );

	std::string mdpContent = VTX::test::getFileContent( in.inputMdp );

	CHECK( mdpContent.empty() == false );
	// We check if tokens are not there
	CHECK( mdpContent.find( "${emtol}" ) == std::string::npos );
	CHECK( mdpContent.find( "${emstep}" ) == std::string::npos );
	CHECK( mdpContent.find( "${nsteps}" ) == std::string::npos );
	// We check if values have been put in the right place
	RE2 patternEmtol { fmt::format( "emtol += +{}", in.min.emtol ) };
	CHECK( RE2::PartialMatch( { mdpContent }, patternEmtol ) );
	RE2 patternEmstep { fmt::format( "emstep += +{}", in.min.emstep ) };
	CHECK( RE2::PartialMatch( { mdpContent }, patternEmstep ) );
	RE2 patternNsteps { fmt::format( "nsteps += +{}", in.min.nsteps ) };
	CHECK( RE2::PartialMatch( { mdpContent }, patternEmstep ) );
}
