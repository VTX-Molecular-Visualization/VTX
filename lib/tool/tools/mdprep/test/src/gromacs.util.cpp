#include "mdprep/test/fixture.hpp"
#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <qapplication.h>
#include <tools/mdprep/gromacs.util.hpp>
#include <tools/mdprep/gromacs/gromacs.args.hpp>
#include <util/logger.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - executable_directory", "[executable_directory]" )
{
	VTX::test::setup_env f;

	const fs::path dir = VTX::Tool::Mdprep::executable_directory();
	CHECK( fs::is_directory( dir ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - list_forcefields empty_dir", "[list_forcefields][empty_dir]" )
{
	VTX::test::setup_env f;

	// tests empty directory scenario
	const fs::path dir = fs::temp_directory_path();
	fs::create_directories( dir );
	CHECK( VTX::Tool::Mdprep::Gromacs::list_forcefields( dir ).empty() );
}

bool is_ff_in_list( const std::vector<VTX::Tool::Mdprep::Gromacs::forcefield> & list, std::string_view name )
{
	return std::find_if(
			   list.begin(),
			   list.end(),
			   [ & ]( const VTX::Tool::Mdprep::Gromacs::forcefield & ff ) { return ff.get_name() == name; }
		   )
		   != list.end();
}

TEST_CASE( "VTX_TOOL_MdPrep - list_forcefields top_dir", "[list_forcefields][top_dir]" )
{
	VTX::test::setup_env f;

	// tests top directory (the default one provided by gromacs) directory scenario
	//  this test is designed to be still true if gromacs maintainers add forcefields.
	//  therefore,the size of the collection shall not be tested
	auto ffs = VTX::Tool::Mdprep::Gromacs::list_forcefields(
		VTX::Tool::Mdprep::executable_directory() / VTX::Tool::Mdprep::Gromacs::default_ff_directory_relative_path()
	);
	CHECK( !ffs.empty() );
	CHECK( is_ff_in_list( ffs, "amber03" ) );
	CHECK( is_ff_in_list( ffs, "amber94" ) );
	CHECK( is_ff_in_list( ffs, "amber96" ) );
	CHECK( is_ff_in_list( ffs, "amber99" ) );
	CHECK( is_ff_in_list( ffs, "amber99sb" ) );
	CHECK( is_ff_in_list( ffs, "amber99sb-ildn" ) );
	CHECK( is_ff_in_list( ffs, "amberGS" ) );
	CHECK( is_ff_in_list( ffs, "charmm27" ) );
	CHECK( is_ff_in_list( ffs, "gromos43a1" ) );
	CHECK( is_ff_in_list( ffs, "gromos43a2" ) );
	CHECK( is_ff_in_list( ffs, "gromos45a3" ) );
	CHECK( is_ff_in_list( ffs, "gromos53a5" ) );
	CHECK( is_ff_in_list( ffs, "gromos53a6" ) );
	CHECK( is_ff_in_list( ffs, "gromos54a7" ) );
	CHECK( is_ff_in_list( ffs, "oplsaa" ) );

	CHECK( !is_ff_in_list( ffs, "poney" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[list_forcefields][some_dir]" )
{
	VTX::test::setup_env f;

	// tests directory with something else than forcefield in it
	CHECK( !fs::is_empty( VTX::Tool::Mdprep::executable_directory() ) );

	auto ffs = VTX::Tool::Mdprep::Gromacs::list_forcefields( VTX::Tool::Mdprep::executable_directory() );
	CHECK( ffs.empty() );
}

namespace VTX::test
{

	struct fixture_convert_pdb2gmx
	{
		VTX::test::setup_env							 f;
		const char *									 output_dir_name = "out";
		VTX::Tool::Mdprep::Gromacs::pdb2gmx_instructions instructions;
		VTX::Tool::Mdprep::Gromacs::gromacs_command_args args;
	};
	fixture_convert_pdb2gmx create_correct_in_out()
	{
		fixture_convert_pdb2gmx f;

		f.instructions.forcefields.emplace_back( "./data/forcefield1.ff" );
		f.instructions.forcefields.emplace_back( "./data/forcefield2.ff" );
		f.instructions.forcefields.emplace_back( "./data/poney.ff" );
		f.instructions.forcefield_index = 1;
		f.instructions.water			= VTX::Tool::Mdprep::Gromacs::water_model::spce;
		f.instructions.output_dir		= VTX::Tool::Mdprep::executable_directory() / f.output_dir_name;
		f.instructions.input_pdb		= VTX::Tool::Mdprep::executable_directory() / "data" / "1ubq.pdb";
		f.instructions.root_file_name	= f.instructions.input_pdb.filename().string();
	}
} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][pdb2gmx][empty]" )
{
	VTX::Tool::Mdprep::Gromacs::pdb2gmx_instructions instructions;
	VTX::Tool::Mdprep::Gromacs::gromacs_command_args args;

	VTX::Tool::Mdprep::Gromacs::convert( instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][pdb2gmx][incorrect_forcefield]" )
{
	VTX::test::setup_env f;

	VTX::Tool::Mdprep::Gromacs::pdb2gmx_instructions instructions;
	VTX::Tool::Mdprep::Gromacs::gromacs_command_args args;

	VTX::Tool::Mdprep::Gromacs::convert( instructions, args );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][pdb2gmx][no_input]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][pdb2gmx][no_output]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][pdb2gmx][correct_instruction]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][empty]" )
{
	VTX::test::setup_env f;

	VTX::Tool::Mdprep::Gromacs::solvate_instructions instructions;
	VTX::Tool::Mdprep::Gromacs::gromacs_command_args args;

	VTX::Tool::Mdprep::Gromacs::convert( instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_input]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_output]" ) {}
