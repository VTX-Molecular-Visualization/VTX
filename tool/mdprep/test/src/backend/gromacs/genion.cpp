#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tool/mdprep/backends/gromacs/genion.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"

TEST_CASE( "VTX_TOOL_MdPrep - genion - prepareJob", "[prepareJob][genion]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GenionInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "genion", "6_genion" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	REQUIRE( in.outputGro.empty() == false );
	REQUIRE( in.inputTop.empty() == false );
	CHECK( fs::exists( in.inputTop ) );
	REQUIRE( in.inputTpr.empty() == false );
	CHECK( fs::exists( in.inputTpr ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - genion - convert", "[convert][genion]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GenionInstructions in;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "genion", "6_genion" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "genion" );
	expectedOutput.arguments.push_back( "-s" );
	expectedOutput.arguments.push_back( in.inputTpr.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputGro.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-pname" );
	expectedOutput.arguments.push_back( in.pname );
	expectedOutput.arguments.push_back( "-pq" );
	expectedOutput.arguments.push_back( std::to_string( in.pq ) );
	expectedOutput.arguments.push_back( "-nname" );
	expectedOutput.arguments.push_back( in.nname );
	expectedOutput.arguments.push_back( "-nq" );
	expectedOutput.arguments.push_back( std::to_string( in.nq ) );
	expectedOutput.arguments.push_back( "-neutral" );
	expectedOutput.arguments.push_back( "-group" );
	expectedOutput.arguments.push_back( "SOL" );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - genion - convert - conc", "[convert][genion]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GenionInstructions in;
	in.conc		= 0.16;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "genion", "6_genion" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "genion" );
	expectedOutput.arguments.push_back( "-s" );
	expectedOutput.arguments.push_back( in.inputTpr.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputGro.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-pname" );
	expectedOutput.arguments.push_back( in.pname );
	expectedOutput.arguments.push_back( "-pq" );
	expectedOutput.arguments.push_back( std::to_string( in.pq ) );
	expectedOutput.arguments.push_back( "-nname" );
	expectedOutput.arguments.push_back( in.nname );
	expectedOutput.arguments.push_back( "-nq" );
	expectedOutput.arguments.push_back( std::to_string( in.nq ) );
	expectedOutput.arguments.push_back( "-neutral" );
	expectedOutput.arguments.push_back( "-conc" );
	expectedOutput.arguments.push_back( std::to_string( in.conc.value() ) );
	expectedOutput.arguments.push_back( "-group" );
	expectedOutput.arguments.push_back( "SOL" );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - genion - submitGromacsJob", "[submitGromacsJob][genion]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GenionInstructions in;
	in.conc		= 0.16;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "genion", "6_genion" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
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
