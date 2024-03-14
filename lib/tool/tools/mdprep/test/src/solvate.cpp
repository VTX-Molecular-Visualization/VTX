#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs/solvate.hpp>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>
#include <tools/mdprep/gromacs/util.hpp>
//
#include <mdprep/test/fixture.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - solvate - prepareJob", "[prepareJob][solvate]" )
{
	// need new folder as usual, but a copy of the topology file into it as well
	using namespace VTX::Tool::Mdprep::Gromacs;
	SolvateInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup		 f( "solvate", "3_solvate" );
	std::vector<const std::string *> previousOutputs;
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	CHECK( fs::exists( f.jobDir ) );
	CHECK( in.inputGro.empty() == false );
	CHECK( in.inputGro.string().ends_with( ".gro" ) );
	CHECK( in.outputGro.empty() == false );
	CHECK( in.outputGro.string().ends_with( "1ubq.gro" ) );
	CHECK( in.outputGro.parent_path() == f.jobDir );
}
