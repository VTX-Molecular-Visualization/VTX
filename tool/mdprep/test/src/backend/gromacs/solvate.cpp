#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tool/mdprep/backends/gromacs/solvate.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"

TEST_CASE( "VTX_TOOL_MdPrep - solvate - prepareJob", "[prepareJob][solvate]" )
{
	// need new folder as usual, but a copy of the topology file into it as well
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	SolvateInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "solvate", "3_solvate" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	CHECK( fs::exists( f.jobDir ) );
	CHECK( fs::exists( in.inputTop ) );
	CHECK( in.inputGro.empty() == false );
	CHECK( in.inputGro.string().ends_with( ".gro" ) );
	CHECK( in.outputGro.empty() == false );
	CHECK( in.outputGro.string().ends_with( "1ubq.gro" ) );
	CHECK( in.outputGro.parent_path() == f.jobDir );
}

TEST_CASE( "VTX_TOOL_MdPrep - solvate - convert", "[convert][solvate]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	SolvateInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "solvate", "3_solvate" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "solvate" );
	expectedOutput.arguments.push_back( "-cs" );
	expectedOutput.arguments.push_back( "spc216.gro" );
	expectedOutput.arguments.push_back( "-cp" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputGro.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - solvate + submitGromacsJob", "[submitGromacsJob][solvate]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	SolvateInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "solvate", "3_solvate" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );
	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	GromacsJobData jobData;
	convert( in, jobData );

	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
	submitGromacsJob( VTX::Tool::Mdprep::executableDirectory() / defaultGmxBinaryRelativePath(), jobData );

	checkJobResults( jobData );
	CHECK( jobData.report.errorOccured == false );
	CHECK( jobData.report.finished == true );
}
