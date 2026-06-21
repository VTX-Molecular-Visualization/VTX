#include "fixture.hpp"
#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <qapplication.h>
#include <unordered_set>
#include <util/logger.hpp>
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/pdb2gmx.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>

namespace VTX::test
{

	struct fixture_convert_pdb2gmx
	{
		VTX::test::setup_env									  f;
		VTX::Tool::Mdprep::backends::Gromacs::Pdb2gmxInstructions instructions;
		VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData	  expectedArgs;
	};
	const fs::path			g_outputDirName = VTX::Tool::Mdprep::executableDirectory() / "out" / "pdb2gmx";
	fixture_convert_pdb2gmx create_correct_in_out()
	{
		fixture_convert_pdb2gmx f;

		f.instructions.forcefields.emplace_back( "./data/forcefield1.ff" );
		f.instructions.forcefields.emplace_back( "./data/forcefield2.ff" );
		f.instructions.forcefields.emplace_back( "./data/poney.ff" );
		f.instructions.forcefieldIndex = 1;
		f.instructions.water		   = VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::spce;
		f.instructions.outputDir	   = g_outputDirName;
		f.instructions.inputPdb		   = VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb";
		f.instructions.fileStem		   = f.instructions.inputPdb.filename().string();

		// I kind of re-do the implementation of convert here, but I guess that if I do it twice, it is half the chance
		// of doing a mistake ?
		f.expectedArgs.arguments.push_back( "pdb2gmx" );
		f.expectedArgs.arguments.push_back( "-f" );
		f.expectedArgs.arguments.push_back( f.instructions.inputPdb.string() );
		std::string inputRootName = f.instructions.inputPdb.filename().string();
		f.expectedArgs.arguments.push_back( "-o" );
		f.expectedArgs.arguments.push_back( ( f.instructions.outputDir / ( inputRootName + ".gro" ) ).string() );
		f.expectedArgs.expectedOutputFilesIndexes.push_back( f.expectedArgs.arguments.size() - 1 );
		f.expectedArgs.arguments.push_back( "-p" );
		f.expectedArgs.arguments.push_back( ( f.instructions.outputDir / ( inputRootName + ".top" ) ).string() );
		f.expectedArgs.expectedOutputFilesIndexes.push_back( f.expectedArgs.arguments.size() - 1 );
		f.expectedArgs.arguments.push_back( "-i" );
		f.expectedArgs.arguments.push_back( ( f.instructions.outputDir / ( inputRootName + ".itp" ) ).string() );
		f.expectedArgs.arguments.push_back( "-n" );
		f.expectedArgs.arguments.push_back( ( f.instructions.outputDir / ( inputRootName + ".ndx" ) ).string() );
		f.expectedArgs.expectedOutputFilesIndexes.push_back( f.expectedArgs.arguments.size() - 1 );
		f.expectedArgs.arguments.push_back( "-ff" );
		auto ffName = f.instructions.forcefields.at( f.instructions.forcefieldIndex ).getName();
		f.expectedArgs.arguments.push_back( std::string { ffName.data(), ffName.size() } );
		f.expectedArgs.arguments.push_back( "-water" );
		f.expectedArgs.arguments.push_back( VTX::Tool::Mdprep::backends::Gromacs::string( f.instructions.water ) );
		return f;
	}

} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - prepareJob - empty", "[prepareJob][pdb2gmx]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	Pdb2gmxInstructions instr;
	instr.inputPdb		 = VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb";
	instr.fileStem		 = "1ubq";
	fs::path	 rootDir = VTX::test::g_outputDirName / "prepareJob";
	const char * jobName = "1-pdb2gmx";
	if ( fs::exists( rootDir / jobName ) )
		fs::remove_all( rootDir / jobName );

	prepareJob( {}, rootDir, jobName, instr );

	CHECK( ( fs::exists( rootDir / jobName ) && fs::is_directory( rootDir / jobName ) ) );
	CHECK( instr.inputPdb == ( VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb" ) );
	CHECK( fs::exists( VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb" ) );
	CHECK( instr.outputDir == ( rootDir / jobName ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - empty", "[convert][pdb2gmx][empty]" )
{
	VTX::Tool::Mdprep::backends::Gromacs::Pdb2gmxInstructions instructions;
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData	  args;

	VTX::Tool::Mdprep::backends::Gromacs::convert( instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - incorrect_forcefield", "[convert][pdb2gmx][incorrect_forcefield]" )
{
	VTX::test::fixture_convert_pdb2gmx					 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;

	data.instructions.forcefieldIndex = 13;

	VTX::Tool::Mdprep::backends::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_input", "[convert][pdb2gmx][no_input]" )
{
	VTX::test::fixture_convert_pdb2gmx					 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;

	data.instructions.inputPdb = "";

	VTX::Tool::Mdprep::backends::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

bool share_same_parent( const fs::path & l, const fs::path & r ) { return l.parent_path() == r.parent_path(); }

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_output", "[convert][pdb2gmx][no_output]" )
{
	VTX::test::fixture_convert_pdb2gmx					 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;

	data.instructions.outputDir = "";

	VTX::Tool::Mdprep::backends::Gromacs::convert( data.instructions, args );

	// This design might be anoying as it is tightly coupled with the implementation of convert. I might want to change
	// that in the future to have a more comprehensive way to retrieve output argument indexes.
	REQUIRE( ( args.expectedOutputFilesIndexes.empty() == false ) );
	for ( auto & outputIdx : args.expectedOutputFilesIndexes )
	{
		CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( outputIdx ) } ) );
	}
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - correct_instruction", "[convert][pdb2gmx][correct_instruction]" )
{
	VTX::test::fixture_convert_pdb2gmx					 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;

	VTX::Tool::Mdprep::backends::Gromacs::convert( data.instructions, args );

	CHECK( data.expectedArgs.arguments == args.arguments );
}
TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - each_interactive_kw", "[convert][pdb2gmx][each_interactive_kw]" )
{
	VTX::test::fixture_convert_pdb2gmx					 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;
	VTX::Tool::Mdprep::backends::Gromacs::Pdb2gmxInputId id {
		'A', VTX::Tool::Mdprep::backends::Gromacs::E_INTERACTIVE_KEYWORD::none, 12
	};
	data.instructions.customParameter.emplace();

	for ( int kwIdx = 0; kwIdx < static_cast<int>( VTX::Tool::Mdprep::backends::Gromacs::E_INTERACTIVE_KEYWORD::COUNT );
		  kwIdx++ )
	{
		VTX::Tool::Mdprep::backends::Gromacs::E_INTERACTIVE_KEYWORD kw
			= static_cast<VTX::Tool::Mdprep::backends::Gromacs::E_INTERACTIVE_KEYWORD>( kwIdx );
		id.kw = kw;
		data.instructions.customParameter->kwValue.clear();
		data.instructions.customParameter->kwValue.insert( { id, "0" } );

		VTX::Tool::Mdprep::backends::Gromacs::convert( data.instructions, args );

		// A -batch flag must always be present when customParameter is set.
		auto batchIt = std::find( args.arguments.begin(), args.arguments.end(), "-batch" );
		REQUIRE( batchIt != args.arguments.end() );
		auto batchPathIt = std::next( batchIt );
		REQUIRE( batchPathIt != args.arguments.end() );
		CHECK( fs::exists( fs::path( *batchPathIt ) ) );
	}
}
