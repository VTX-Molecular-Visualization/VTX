#include <tools/mdprep/gromacs/inputs.hpp>
//
#include "mdprep/test/fixture.hpp"
#include <algorithm>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <qapplication.h>
#include <unordered_set>
#include <util/logger.hpp>
//
#include <tools/mdprep/gromacs/gromacs.hpp>
#include <tools/mdprep/gromacs/pdb2gmx.hpp>
#include <tools/mdprep/gromacs/util.hpp>

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
	CHECK( !is_ff_in_list( ffs, "amber96poney" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[listForcefields][some_dir]" )
{
	VTX::test::setup_env f;

	// tests directory with something else than forcefield in it
	CHECK( !fs::is_empty( VTX::Tool::Mdprep::executableDirectory() ) );

	auto ffs = VTX::Tool::Mdprep::Gromacs::listForcefields( VTX::Tool::Mdprep::executableDirectory() );
	CHECK( ffs.empty() );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript", "[pdb2gmx][Pdb2gmxInputId][hash]" )
{
	// We want to test if the hash function for Pdb2gmxInputId datastruct gives unique hash with plausible inputs
	const size_t MAX_RESID = 1001;

	using namespace VTX::Tool::Mdprep::Gromacs;

	Pdb2gmxInputId					   id;
	bool							   duplicateFound = false;
	std::unordered_set<Pdb2gmxInputId> set;
	for ( char itChar = 'A'; itChar <= 'Z'; itChar++ )
	{
		for ( uint32_t it_kw_num = 0; it_kw_num < static_cast<uint32_t>( E_INTERACTIVE_KEYWORD::COUNT ); it_kw_num++ )
		{
			E_INTERACTIVE_KEYWORD it_kw = static_cast<E_INTERACTIVE_KEYWORD>( it_kw_num );
			for ( uint32_t it_num = 0; it_num < MAX_RESID; it_num++ )
			{
				id.chain = itChar;
				id.kw	 = it_kw;
				id.num	 = it_num;

				if ( set.contains( id ) )
				{
					duplicateFound = true;
					break;
				}
				set.insert( id );
			}
			if ( duplicateFound )
				break;
		}
		if ( duplicateFound )
			break;
	}
	CHECK( duplicateFound == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - empty", "[pdb2gmx][parsePdb2gmxUserScript][empty]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args, expectedArgs;
	const char *  script = "";
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - one line", "[pdb2gmx][parsePdb2gmxUserScript][one_line]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script = "A ARG54 0\n";
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - one line, no line feed",
	"[pdb2gmx][parsePdb2gmxUserScript][one_line]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script = "A ARG54 0";
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::arg, 54 }, "0" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - 1", "[pdb2gmx][parsePdb2gmxUserScript][1]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script
		= "A LYS222 LYN\n"
		  "B LYS8 LYS\n"
		  "A HIS1 1\n"
		  "A HIS12 HID\n";
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'B', E_INTERACTIVE_KEYWORD::lys, 8 }, "LYS" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::lys, 222 }, "LYN" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::his, 1 }, "1" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}

TEST_CASE(
	"VTX_TOOL_MdPrep - parsePdb2gmxUserScript - carriage_return",
	"[pdb2gmx][parsePdb2gmxUserScript][carriage_return]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script
		= "A LYS222 LYN\r\n"
		  "B LYS8 LYS\r\n"
		  "A HIS1 1\r\n"
		  "A HIS12 HID\r\n";
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'B', E_INTERACTIVE_KEYWORD::lys, 8 }, "LYS" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::lys, 222 }, "LYN" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::his, 1 }, "1" );
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::his, 12 }, "HID" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}
TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - value_lower", "[pdb2gmx][parsePdb2gmxUserScript][value_lower]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script = "A HIS8 hie\r\n";
	expectedArgs.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::his, 8 }, "hie" );
	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}
void data_each( const char *& s, VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputs & args ) noexcept
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
	args.kwValue.emplace( Pdb2gmxInputId { 'A', E_INTERACTIVE_KEYWORD::ss, 111 }, "0" );
	args.kwValue.emplace( Pdb2gmxInputId { 'B', E_INTERACTIVE_KEYWORD::ter, 112 }, "1" );
	args.kwValue.emplace( Pdb2gmxInputId { 'C', E_INTERACTIVE_KEYWORD::lys, 113 }, "protonated" );
	args.kwValue.emplace( Pdb2gmxInputId { 'D', E_INTERACTIVE_KEYWORD::arg, 114 }, "1" );
	args.kwValue.emplace( Pdb2gmxInputId { 'E', E_INTERACTIVE_KEYWORD::asp, 115 }, "not protonated" );
	args.kwValue.emplace( Pdb2gmxInputId { 'F', E_INTERACTIVE_KEYWORD::glu, 116 }, "not protonated" );
	args.kwValue.emplace( Pdb2gmxInputId { 'G', E_INTERACTIVE_KEYWORD::gln, 117 }, "GLN" );
	args.kwValue.emplace( Pdb2gmxInputId { 'H', E_INTERACTIVE_KEYWORD::his, 118 }, "HIE" );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - each", "[pdb2gmx][parsePdb2gmxUserScript][each]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	Pdb2gmxInputs expectedArgs;
	const char *  script = nullptr;

	data_each( script, expectedArgs );

	VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( args == expectedArgs );
}

TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - error 1", "[pdb2gmx][parsePdb2gmxUserScript][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	const char *  script = "hehe lol";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - parsePdb2gmxUserScript - error 2", "[pdb2gmx][parsePdb2gmxUserScript][error]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	Pdb2gmxInputs args;
	const char *  script = "A ss111 0 B ter112 1";

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
	Pdb2gmxInputs args;
	const char *  script
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
	Pdb2gmxInputs args;
	const char *  script
		= "BA te112 1\n"
		  "C lys113 protonated\n";

	auto report = VTX::Tool::Mdprep::Gromacs::parsePdb2gmxUserScript( script, args );

	CHECK( report.error );
	CHECK( report.message.empty() == false );
}