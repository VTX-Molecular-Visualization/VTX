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

TEST_CASE( "VTX_TOOL_MdPrep - executableDirectory", "[executableDirectory]" )
{
	VTX::test::setup_env f;

	const fs::path dir = VTX::Tool::Mdprep::executableDirectory();
	CHECK( fs::is_directory( dir ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - listForcefields empty_dir", "[listForcefields][empty_dir]" )
{
	VTX::test::setup_env f;

	// tests empty directory scenario
	const fs::path dir = fs::temp_directory_path();
	fs::create_directories( dir );
	CHECK( VTX::Tool::Mdprep::Gromacs::listForcefields( dir ).empty() );
}

bool is_ff_in_list( const std::vector<VTX::Tool::Mdprep::Gromacs::forcefield> & list, std::string_view name )
{
	return std::find_if(
			   list.begin(),
			   list.end(),
			   [ & ]( const VTX::Tool::Mdprep::Gromacs::forcefield & ff ) { return ff.getName() == name; }
		   )
		   != list.end();
}

TEST_CASE( "VTX_TOOL_MdPrep - listForcefields top_dir", "[listForcefields][top_dir]" )
{
	VTX::test::setup_env f;

	// tests top directory (the default one provided by gromacs) directory scenario
	//  this test is designed to be still true if gromacs maintainers add forcefields.
	//  therefore,the size of the collection shall not be tested
	auto ffs = VTX::Tool::Mdprep::Gromacs::listForcefields(
		VTX::Tool::Mdprep::executableDirectory() / VTX::Tool::Mdprep::Gromacs::defaultFfDirectoryRelativePath()
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

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[listForcefields][some_dir]" )
{
	VTX::test::setup_env f;

	// tests directory with something else than forcefield in it
	CHECK( !fs::is_empty( VTX::Tool::Mdprep::executableDirectory() ) );

	auto ffs = VTX::Tool::Mdprep::Gromacs::listForcefields( VTX::Tool::Mdprep::executableDirectory() );
	CHECK( ffs.empty() );
}

namespace VTX::test
{

	struct fixture_convert_pdb2gmx
	{
		VTX::test::setup_env							 f;
		const char *									 outputDir_name = "out";
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInstructions instructions;
		VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs expected_args;
	};
	fixture_convert_pdb2gmx create_correct_in_out()
	{
		fixture_convert_pdb2gmx f;

		f.instructions.forcefields.emplace_back( "./data/forcefield1.ff" );
		f.instructions.forcefields.emplace_back( "./data/forcefield2.ff" );
		f.instructions.forcefields.emplace_back( "./data/poney.ff" );
		f.instructions.forcefieldIndex = 1;
		f.instructions.water			= VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::spce;
		f.instructions.outputDir		= VTX::Tool::Mdprep::executableDirectory() / f.outputDir_name;
		f.instructions.inputPdb		= VTX::Tool::Mdprep::executableDirectory() / "data" / "1ubq.pdb";
		f.instructions.rootFileName	= f.instructions.inputPdb.filename().string();

		// I kind of re-do the implementation of convert here, but I guess that if I do it twice, it is half the chance
		// of doing a mistake ?
		f.expected_args.arguments.push_back( "pdb2gmx" );
		f.expected_args.arguments.push_back( "-f" );
		f.expected_args.arguments.push_back( f.instructions.inputPdb.string() );
		std::string input_root_name = f.instructions.inputPdb.filename().string();
		f.expected_args.arguments.push_back( "-o" );
		f.expected_args.arguments.push_back( ( f.instructions.outputDir / ( input_root_name + ".gro" ) ).string() );
		f.expected_args.arguments.push_back( "-p" );
		f.expected_args.arguments.push_back( ( f.instructions.outputDir / ( input_root_name + ".top" ) ).string() );
		f.expected_args.arguments.push_back( "-i" );
		f.expected_args.arguments.push_back( ( f.instructions.outputDir / ( input_root_name + ".itp" ) ).string() );
		f.expected_args.arguments.push_back( "-q" );
		f.expected_args.arguments.push_back( ( f.instructions.outputDir / ( input_root_name + ".clean.pdb" ) ).string()
		);
		f.expected_args.arguments.push_back( "-n" );
		f.expected_args.arguments.push_back( ( f.instructions.outputDir / ( input_root_name + ".ndx" ) ).string() );
		f.expected_args.arguments.push_back( "-ff" );
		f.expected_args.arguments.push_back(
			f.instructions.forcefields.at( f.instructions.forcefieldIndex ).getName().data()
		);
		f.expected_args.arguments.push_back( "-water" );
		f.expected_args.arguments.push_back( VTX::Tool::Mdprep::Gromacs::string( f.instructions.water ) );
		return f;
	}
} // namespace VTX::test

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - empty", "[convert][pdb2gmx][empty]" )
{
	VTX::Tool::Mdprep::Gromacs::Pdb2gmxInstructions instructions;
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	VTX::Tool::Mdprep::Gromacs::convert( instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - incorrect_forcefield", "[convert][pdb2gmx][incorrect_forcefield]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.forcefieldIndex = 13;

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_input", "[convert][pdb2gmx][no_input]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.inputPdb = "";

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	CHECK( args.arguments.empty() );
}

bool share_same_parent( const fs::path & l, const fs::path & r ) { return l.parent_path() == r.parent_path(); }

TEST_CASE( "VTX_TOOL_MdPrep - pdb2gmx - convert - no_output", "[convert][pdb2gmx][no_output]" )
{
	VTX::test::fixture_convert_pdb2gmx				 data = VTX::test::create_correct_in_out();
	VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;

	data.instructions.outputDir = "";

	VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );

	// This design might be anoying as it is tightly coupled with the implementation of convert. I might want to change
	// that in the future to have a more comprehensive way to retrieve output argument indexes.
	CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( 4 ) } ) );
	CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( 6 ) } ) );
	CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( 8 ) } ) );
	CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( 10 ) } ) );
	CHECK( share_same_parent( data.instructions.inputPdb, { args.arguments.at( 12 ) } ) );
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
	VTX::Tool::Mdprep::Gromacs::InteractiveId id { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::none, 12 };
	data.instructions.customParameter.emplace();

	data.expected_args.arguments.push_back( "" );
	for ( int kw_idx = 0; kw_idx < static_cast<int>( VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::COUNT );
		  kw_idx++ )
	{
		VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD kw
			= static_cast<VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD>( kw_idx );
		id.kw = kw;
		data.instructions.customParameter->kwValue.clear();
		data.instructions.customParameter->kwValue.insert( { id, "0" } );
		data.expected_args.arguments.back() = std::string( "-" ) += VTX::Tool::Mdprep::Gromacs::string( kw );

		VTX::Tool::Mdprep::Gromacs::convert( data.instructions, args );
		data.expected_args.interactiveSettings = args.interactiveSettings;
		CHECK( data.expected_args == args );
	}
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript", "[pdb2gmx][InteractiveId][hash]" )
{
	// We want to test if the hash function for InteractiveId datastruct gives unique hash with plausible inputs
	const size_t MAX_RESID = 1001;

	using namespace VTX::Tool::Mdprep::Gromacs;

	InteractiveId					   id;
	bool							   duplicate_found = false;
	std::unordered_set<InteractiveId> set;
	for ( char it_char = 'A'; it_char <= 'Z'; it_char++ )
	{
		for ( uint32_t it_kw_num = 0; it_kw_num < static_cast<uint32_t>( E_INTERACTIVE_KEYWORD::COUNT ); it_kw_num++ )
		{
			E_INTERACTIVE_KEYWORD it_kw = static_cast<E_INTERACTIVE_KEYWORD>( it_kw_num );
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
TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - empty", "[pdb2gmx][parsePdb2gmxUserScript][empty]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args, expected_args;
	const char *		  script = "";
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - one line", "[pdb2gmx][parsePdb2gmxUserScript][one_line]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A ARG54 0\n";
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - one line, no line feed",
	"[pdb2gmx][parsePdb2gmxUserScript][one_line]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A ARG54 0";
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - 1", "[pdb2gmx][parsePdb2gmxUserScript][1]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script
		= "A LYS222 LYN\n"
		  "B LYS8 LYS\n"
		  "A HIS1 1\n"
		  "A HIS12 HID\n";
	expected_args.kwValue.emplace( InteractiveId { 'B', E_INTERACTIVE_KEYWORD::lys, 8 }, "LYS" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 222 }, "LYN" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::his, 1 }, "1" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - carriage_return",
	"[pdb2gmx][parsePdb2gmxUserScript][carriage_return]"
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
	expected_args.kwValue.emplace( InteractiveId { 'B', E_INTERACTIVE_KEYWORD::lys, 8 }, "LYS" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 222 }, "LYN" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::his, 1 }, "1" );
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - value_lower",
	"[pdb2gmx][parsePdb2gmxUserScript][value_lower]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = "A HIS8 hie\r\n";
	expected_args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::his, 8 }, "hie" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

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
	args.kwValue.clear();
	args.kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::ss, 111 }, "0" );
	args.kwValue.emplace( InteractiveId { 'B', E_INTERACTIVE_KEYWORD::ter, 112 }, "1" );
	args.kwValue.emplace( InteractiveId { 'C', E_INTERACTIVE_KEYWORD::lys, 113 }, "protonated" );
	args.kwValue.emplace( InteractiveId { 'D', E_INTERACTIVE_KEYWORD::arg, 114 }, "1" );
	args.kwValue.emplace( InteractiveId { 'E', E_INTERACTIVE_KEYWORD::asp, 115 }, "not protonated" );
	args.kwValue.emplace( InteractiveId { 'F', E_INTERACTIVE_KEYWORD::glu, 116 }, "not protonated" );
	args.kwValue.emplace( InteractiveId { 'G', E_INTERACTIVE_KEYWORD::gln, 117 }, "GLN" );
	args.kwValue.emplace( InteractiveId { 'H', E_INTERACTIVE_KEYWORD::his, 118 }, "HIE" );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - each", "[pdb2gmx][parsePdb2gmxUserScript][each]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	InteractiveArguments expected_args;
	const char *		  script = nullptr;

	data_each( script, expected_args );

	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expected_args );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - error 1", "[pdb2gmx][parsePdb2gmxUserScript][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script = "hehe lol";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - error 2", "[pdb2gmx][parsePdb2gmxUserScript][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script = "A ss111 0 B ter112 1";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - error bad residue name",
	"[pdb2gmx][parsePdb2gmxUserScript][error]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script
		= "B te112 1\n"
		  "C lys113 protonated\n";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - chain name too long",
	"[pdb2gmx][parsePdb2gmxUserScript][error]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	InteractiveArguments args;
	const char *		  script
		= "BA te112 1\n"
		  "C lys113 protonated\n";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][empty]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_input]" ) {}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[convert][solvate][no_output]" ) {}
