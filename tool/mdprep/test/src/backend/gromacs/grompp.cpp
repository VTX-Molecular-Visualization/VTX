#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
//
#include <tool/mdprep/backends/gromacs/grompp.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"

TEST_CASE( "VTX_TOOL_MdPrep - grompp - prepareJob - ions", "[prepareJob][grompp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		  = E_GROMPP_STEP::ions;
	in.min.emtol  = 666.6;
	in.min.emstep = 0.12;
	in.min.nsteps = 6028;
	in.fileStem	  = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "5_grompp" );
	CumulativeOuputFiles	   previousOutputs;
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

	REQUIRE( in.inputGro.empty() == false );
	CHECK( fs::exists( in.inputGro ) );
	REQUIRE( in.inputMdp.empty() == false );
	CHECK( fs::exists( in.inputMdp ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - grompp - prepareJob - em", "[prepareJob][grompp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		  = E_GROMPP_STEP::em;
	in.min.emtol  = 777.0;
	in.min.emstep = 0.02;
	in.min.nsteps = 823;
	in.fileStem	  = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "7_grompp" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Genion::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( { previousOutputs }, f.rootDir, f.jobName, in );

	CHECK( in.outputMdp.empty() == false );
	CHECK( in.outputMdp.string().ends_with( in.fileStem + ".mdp" ) == true );

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
	CHECK( RE2::PartialMatch( { mdpContent }, patternNsteps ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - grompp - convert - em", "[convert][grompp][em]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		= E_GROMPP_STEP::em;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "5_grompp" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Genion::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( previousOutputs, f.rootDir, f.jobName, in );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "grompp" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.inputMdp.string() );
	expectedOutput.arguments.push_back( "-c" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-r" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	expectedOutput.arguments.push_back( "-po" );
	expectedOutput.arguments.push_back( in.outputMdp.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-pp" );
	expectedOutput.arguments.push_back( in.outputTop.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputTpr.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-maxwarn" );
	expectedOutput.arguments.push_back( "1" );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - grompp - convert - ions", "[convert][grompp][ions]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		= E_GROMPP_STEP::ions;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "5_grompp" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	prepareJob( previousOutputs, f.rootDir, f.jobName, in );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "grompp" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.inputMdp.string() );
	expectedOutput.arguments.push_back( "-c" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-r" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	expectedOutput.arguments.push_back( "-po" );
	expectedOutput.arguments.push_back( in.outputMdp.string() );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputTpr.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-maxwarn" );
	expectedOutput.arguments.push_back( "1" );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - grompp - convert - posres", "[convert][grompp][posres]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		= E_GROMPP_STEP::posres;
	in.fileStem = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "5_grompp" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	GromacsJobData expectedOutput, actualOutput;
	expectedOutput.arguments.push_back( "grompp" );
	expectedOutput.arguments.push_back( "-f" );
	expectedOutput.arguments.push_back( in.inputMdp.string() );
	expectedOutput.arguments.push_back( "-c" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-r" );
	expectedOutput.arguments.push_back( in.inputGro.string() );
	expectedOutput.arguments.push_back( "-p" );
	expectedOutput.arguments.push_back( in.inputTop.string() );
	expectedOutput.arguments.push_back( "-po" );
	expectedOutput.arguments.push_back( in.outputMdp.string() );
	expectedOutput.arguments.push_back( "-pp" );
	expectedOutput.arguments.push_back( in.outputTop.string() );
	setLastArgumentAsExpectedOutputFile( expectedOutput );
	expectedOutput.arguments.push_back( "-o" );
	expectedOutput.arguments.push_back( in.outputTpr.string() );
	expectedOutput.arguments.push_back( "-maxwarn" );
	expectedOutput.arguments.push_back( "1" );

	convert( in, actualOutput );

	CHECK( expectedOutput.arguments == actualOutput.arguments );
	CHECK( expectedOutput.expectedOutputFilesIndexes == actualOutput.expectedOutputFilesIndexes );
}

TEST_CASE( "VTX_TOOL_MdPrep - grompp + submitGromacsJob - ions", "[submitGromacsJob][grompp][ions]" )
{
	return; // TMP TODO : Put it back online

	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.step		  = E_GROMPP_STEP::ions;
	in.min.emtol  = 666.6;
	in.min.emstep = 0.12;
	in.min.nsteps = 6028;
	in.fileStem	  = "1ubq";
	VTX::test::PrepareJobSetup f( "grompp", "5_grompp" );
	CumulativeOuputFiles	   previousOutputs;
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

TEST_CASE( "VTX_TOOL_MdPrep - grompp + submitGromacsJob - em", "[submitGromacsJob][grompp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	GromppInstructions in;
	in.fileStem = "1ubq";
	in.step		= E_GROMPP_STEP::em;
	VTX::test::PrepareJobSetup f( "grompp", "7_grompp" );
	CumulativeOuputFiles	   previousOutputs;
	VTX::test::fill( VTX::test::Genion::g_fileList, previousOutputs );
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

	std::string outputs { "Here is the stdout and stderr : \n" };
	outputs += jobData.channelsLocker.open()->stdout_;
	outputs += jobData.channelsLocker.open()->stderr_;
	CAPTURE( outputs );
	INFO( outputs );
	CHECK( jobData.report.errorOccured == false );
	CHECK( jobData.report.finished == true );
}
