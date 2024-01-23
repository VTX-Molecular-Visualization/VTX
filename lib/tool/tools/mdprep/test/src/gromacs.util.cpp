#include "mdprep/test/fixture.hpp"
#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <qapplication.h>
#include <unordered_set>
#include <util/logger.hpp>
//
#include <tools/mdprep/gromacs.hpp>
#include <tools/mdprep/gromacs.util.hpp>

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
		VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs expected_args;
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

		// I kind of re-do the implementation of convert here, but I guess that if I do it twice, it is half the chance
		// of doing a mistake ?
		f.expected_args.arguments.push_back( "pdb2gmx" );
		f.expected_args.arguments.push_back( "-f" );
		f.expected_args.arguments.push_back( f.instructions.input_pdb.string() );
		std::string input_root_name = f.instructions.input_pdb.filename().string();
		f.expected_args.arguments.push_back( "-o" );
		f.expected_args.arguments.push_back( ( f.instructions.output_dir / ( input_root_name + ".gro" ) ).string() );
		f.expected_args.arguments.push_back( "-p" );
		f.expected_args.arguments.push_back( ( f.instructions.output_dir / ( input_root_name + ".top" ) ).string() );
		f.expected_args.arguments.push_back( "-i" );
		f.expected_args.arguments.push_back( ( f.instructions.output_dir / ( input_root_name + ".itp" ) ).string() );
		f.expected_args.arguments.push_back( "-q" );
		f.expected_args.arguments.push_back( ( f.instructions.output_dir / ( input_root_name + ".clean.pdb" ) ).string()
		);
		f.expected_args.arguments.push_back( "-n" );
		f.expected_args.arguments.push_back( ( f.instructions.output_dir / ( input_root_name + ".ndx" ) ).string() );
		f.expected_args.arguments.push_back( "-ff" );
		f.expected_args.arguments.push_back(
			f.instructions.forcefields.at( f.instructions.forcefield_index ).get_name().data()
		);
		f.expected_args.arguments.push_back( "-water" );
		f.expected_args.arguments.push_back( VTX::Tool::Mdprep::Gromacs::string( f.instructions.water ) );
		return f;
	}
} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - empty", "[convert][pdb2gmx][empty]" )
{
	VTX::Tool::Mdprep::Gromacs::pdb2gmx_instructions instructions;
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	VTX::Tool::Mdprep::Gromacs::convert( instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - incorrect_forcefield", "[convert][pdb2gmx][incorrect_forcefield]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.forcefield_index = 13;

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_input", "[convert][pdb2gmx][no_input]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.input_pdb = "";

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

bool share_same_parent( const fs::path & l, const fs::path & r ) { return l.parent_path() == r.parent_path(); }

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_output", "[convert][pdb2gmx][no_output]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.output_dir = "";

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	// This design might be anoying as it is tightly coupled with the implementation of convert. I might want to change
	// that in the future to have a more comprehensive way to retrieve output argument indexes.
	CHECK( share_same_parent( data.instructions.input_pdb, { args.arguments.at( 4 ) } ) );
	CHECK( share_same_parent( data.instructions.input_pdb, { args.arguments.at( 6 ) } ) );
	CHECK( share_same_parent( data.instructions.input_pdb, { args.arguments.at( 8 ) } ) );
	CHECK( share_same_parent( data.instructions.input_pdb, { args.arguments.at( 10 ) } ) );
	CHECK( share_same_parent( data.instructions.input_pdb, { args.arguments.at( 12 ) } ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - correct_instruction", "[convert][pdb2gmx][correct_instruction]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	CHECK( data.expected_args == args );
}
TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - each_interactive_kw", "[convert][pdb2gmx][each_interactive_kw]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;
	VTX::Tool::Mdprep::Gromacs::InteractiveId id { 'A', VTX::Tool::Mdprep::Gromacs::InteractiveKeyword::none, 12 };
	data.instructions.custom_parameter.emplace();

	data.expected_args.arguments.push_back( "" );
	for ( int kw_idx = 0; kw_idx < static_cast<int>( VTX::Tool::Mdprep::Gromacs::InteractiveKeyword::COUNT );
		  kw_idx++ )
	{
		VTX::Tool::Mdprep::Gromacs::InteractiveKeyword kw
			= static_cast<VTX::Tool::Mdprep::Gromacs::InteractiveKeyword>( kw_idx );
		id.kw = kw;
		data.instructions.custom_parameter->kw_v.clear();
		data.instructions.custom_parameter->kw_v.insert( { id, "0" } );
		data.expected_args.arguments.back() = std::string( "-" ) += VTX::Tool::Mdprep::Gromacs::string( kw );

		VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );
		data.expected_args.interactive_settings = args.interactive_settings;
		CHECK( data.expected_args == args );
	}
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script", "[pdb2gmx][InteractiveId][hash]" )
{
	// We want to test if the hash function for InteractiveId datastruct gives unique hash with plausible inputs
	const size_t MAX_RESID = 1001;

	using namespace VTX::Tool::Mdprep::Gromacs;

	InteractiveId					   id;
	bool							   duplicate_found = false;
	std::unordered_set<InteractiveId> set;
	for ( char it_char = 'A'; it_char <= 'Z'; it_char++ )
	{
		for ( uint32_t it_kw_num = 0; it_kw_num < static_cast<uint32_t>( InteractiveKeyword::COUNT ); it_kw_num++ )
		{
			InteractiveKeyword it_kw = static_cast<InteractiveKeyword>( it_kw_num );
			for ( uint32_t it_num = 0; it_num < MAX_RESID; it_num++ )
			{
				id.chain = it_char;
				id.kw	 = it_kw;
				id.num	 = it_num;

				if ( set.contains( id ) )
				{
					duplicate_found = true;
					break;
				}
				set.insert( id );
			}
			if ( duplicate_found )
				break;
		}
		if ( duplicate_found )
			break;
	}
	CHECK( duplicate_found == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - empty", "[pdb2gmx][parse_pdb2gmx_user_script][empty]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args, expected_args;
	const char *		  script = "";
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - one line", "[pdb2gmx][parse_pdb2gmx_user_script][one_line]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A ARG54 0\n";
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - one line, no line feed",
	"[pdb2gmx][parse_pdb2gmx_user_script][one_line]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A ARG54 0";
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - 1", "[pdb2gmx][parse_pdb2gmx_user_script][1]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script
		= "A LYS222 LYN\n"
		  "B LYS8 LYS\n"
		  "A HIS1 1\n"
		  "A HIS12 HID\n";
	expected_args.kw_v.emplace( InteractiveId { 'B', InteractiveKeyword::lys, 8 }, "LYS" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::lys, 222 }, "LYN" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::his, 1 }, "1" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - carriage_return",
	"[pdb2gmx][parse_pdb2gmx_user_script][carriage_return]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script
		= "A LYS222 LYN\r\n"
		  "B LYS8 LYS\r\n"
		  "A HIS1 1\r\n"
		  "A HIS12 HID\r\n";
	expected_args.kw_v.emplace( InteractiveId { 'B', InteractiveKeyword::lys, 8 }, "LYS" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::lys, 222 }, "LYN" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::his, 1 }, "1" );
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - value_lower",
	"[pdb2gmx][parse_pdb2gmx_user_script][value_lower]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A HIS8 hie\r\n";
	expected_args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::his, 8 }, "hie" );
	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}
void data_each( const char *& s, VTX::Tool::Mdprep::Gromacs::InteractiveArguments & args ) noexcept
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	const char * script
		= "A ss111 0\n"
		  "B ter112 1\n"
		  "C lys113 protonated\n"
		  "D arg114 1\n"
		  "E asp115 not protonated\n"
		  "F glu116 not protonated\n"
		  "G gln117 GLN\n"
		  "H his118 HIE\n";
	s = script;
	args.kw_v.clear();
	args.kw_v.emplace( InteractiveId { 'A', InteractiveKeyword::ss, 111 }, "0" );
	args.kw_v.emplace( InteractiveId { 'B', InteractiveKeyword::ter, 112 }, "1" );
	args.kw_v.emplace( InteractiveId { 'C', InteractiveKeyword::lys, 113 }, "protonated" );
	args.kw_v.emplace( InteractiveId { 'D', InteractiveKeyword::arg, 114 }, "1" );
	args.kw_v.emplace( InteractiveId { 'E', InteractiveKeyword::asp, 115 }, "not protonated" );
	args.kw_v.emplace( InteractiveId { 'F', InteractiveKeyword::glu, 116 }, "not protonated" );
	args.kw_v.emplace( InteractiveId { 'G', InteractiveKeyword::gln, 117 }, "GLN" );
	args.kw_v.emplace( InteractiveId { 'H', InteractiveKeyword::his, 118 }, "HIE" );
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - each", "[pdb2gmx][parse_pdb2gmx_user_script][each]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = nullptr;

	data_each( script, expected_args );

	VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - error 1", "[pdb2gmx][parse_pdb2gmx_user_script][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script = "hehe lol";

	auto report = VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - error 2", "[pdb2gmx][parse_pdb2gmx_user_script][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script = "A ss111 0 B ter112 1";

	auto report = VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - error bad residue name",
	"[pdb2gmx][parse_pdb2gmx_user_script][error]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script
		= "B te112 1\n"
		  "C lys113 protonated\n";

	auto report = VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_pdb2gmx_user_script - chain name too long",
	"[pdb2gmx][parse_pdb2gmx_user_script][error]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script
		= "BA te112 1\n"
		  "C lys113 protonated\n";

	auto report = VTX::Tool::Mdprep::Gromacs::parse_pdb2gmx_user_script( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][empty]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_input]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_output]" ) {}
