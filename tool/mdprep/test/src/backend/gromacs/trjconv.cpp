#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tool/mdprep/backends/gromacs/trjconv.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"

TEST_CASE( "VTX_TOOL_MdPrep - trjconv - prepareJob", "[prepareJob][trjconv]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	TrjconvInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "trjconv", "4_trjconv" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	CHECK( fs::exists( f.jobDir ) );
	CHECK( fs::exists( in.inputGro ) );
	CHECK( in.inputGro.empty() == false );
	CHECK( in.inputGro.string().ends_with( ".gro" ) );
	CHECK( in.outputGro.empty() == false );
	CHECK( in.outputGro.string().ends_with( "1ubq.gro" ) );
	CHECK( in.outputGro.parent_path() == f.jobDir );
}

TEST_CASE( "VTX_TOOL_MdPrep - trjconv - convert", "[convert][trjconv]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	TrjconvInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "trjconv", "4_trjconv" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "trjconv" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-s" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputGro.string() );
	expectedOutput.expectedOutputFilesIndexes.push_back( expectedOutput.arguments.size() - 1 );
	expectedOutput.arguments.push_back( "-center" );
	expectedOutput.arguments.push_back( "-center-group" );
	expectedOutput.arguments.push_back( "Protein" );
	expectedOutput.arguments.push_back( "-output-group" );
	expectedOutput.arguments.push_back( "System" );
	expectedOutput.arguments.push_back( "-pbc" );
	expectedOutput.arguments.push_back( "res" );
	expectedOutput.arguments.push_back( "-ur" );
	expectedOutput.arguments.push_back( "compact" );

	convert( in, actualOutput );

	CHECK( actualOutput.arguments == expectedOutput.arguments );
	CHECK( actualOutput.expectedOutputFilesIndexes == expectedOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - trjconv + submitGromacsJob", "[submitGromacsJob][trjconv]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	TrjconvInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "trjconv", "4_trjconv" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );
	GromacsJobData jobData;

	convert( in, jobData );

	declareFfDirectory( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
	submitGromacsJob( VTX::Tool::Mdprep::executableDirectory() / defaultGmxBinaryRelativePath(), jobData );

	checkJobResults( jobData );
	std::string outputs;
	outputs += jobData.channelsLocker.open()->stdout_;
	outputs += jobData.channelsLocker.open()->stderr_;
	INFO( outputs );
	CHECK( jobData.report.errorOccured == false );
	CHECK( jobData.report.finished == true );
}
