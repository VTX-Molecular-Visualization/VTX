#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs/pdb2gmx.hpp>
#include <tools/mdprep/gromacs/pdb2gmx.impl.hpp>
#include <tools/mdprep/gromacs/util.hpp>

// testing isWaitingInputs

TEST_CASE( "VTX_TOOL_MdPrep - isWaitingInputs - empty", "[isWaitingInputs][empty]" )
{
	std::string stdout_ { "" };
	CHECK( VTX::Tool::Mdprep::Gromacs::isWaitingInputs( stdout_ ) == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - isWaitingInputs - exact_pattern", "[isWaitingInputs][exact_pattern]" )
{
	std::string stdout_ { "\nType a number:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::isWaitingInputs( stdout_ ) == true );
}
TEST_CASE( "VTX_TOOL_MdPrep - isWaitingInputs - garbage+pattern", "[isWaitingInputs][garbage+pattern]" )
{
	std::string stdout_ { "lololol BBQ sauce heh\nType a number:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::isWaitingInputs( stdout_ ) == true );
}
TEST_CASE( "VTX_TOOL_MdPrep - isWaitingInputs - anti-pattern", "[isWaitingInputs][anti-pattern]" )
{
	std::string stdout_ { "lololol BBQ sauce heh\nType a poney:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::isWaitingInputs( stdout_ ) == false );

	stdout_ = "lololol BBQ sauce heh\nType a number:2\nblabla\nsome other writtings\nfancy calculations";
	CHECK( VTX::Tool::Mdprep::Gromacs::isWaitingInputs( stdout_ ) == false );
}

// testing parseExpectedKwArgument

TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - empty", "[parseExpectedKwArgument][empty]" )
{
	std::string								   stdout_ { "" };
	VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId kw;

	CHECK( VTX::Tool::Mdprep::Gromacs::parseExpectedKwArgument( stdout_, kw ) == false );
}
namespace
{
	bool check_parseExpectedKwArgument(
		std::string										   p_stdout,
		const VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId & p_expectedOutput
	)
	{
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId kw;
		return VTX::Tool::Mdprep::Gromacs::parseExpectedKwArgument( p_stdout, kw )
			   && ( kw.chain == p_expectedOutput.chain && kw.kw == p_expectedOutput.kw && kw.num == p_expectedOutput.num
			   );
	}
	const char	 g_chainA[] = "Processing chain 1 'A'( 6457 atoms, 827 residues )\n";
	const char	 g_chainB[] = "Processing chain 1 'B'( 6457 atoms, 827 residues )\n";
	const char * g_lys1
		= "Which LYSINE type do you want for residue 40\n0. Not "
		  "protonated (charge 0) (LYN)\n1. Protonated (charge +1) (LYS)\n\nType a number:";
	const char * g_arg1
		= "Which ARGININE type do you want for residue 31\n0. Not "
		  "protonated (charge 0) (-)\n1. Protonated (charge +1) (ARG)\n\nType a number:";
	const char * g_asp1
		= "Which ASPARTIC ACID type do you want for residue 13\n0. "
		  "Not protonated (charge -1) (ASP)\n1. Protonated (charge 0) (ASH)\n\nType a number:";
	const char * g_glu1
		= "Which GLUTAMIC ACID type do you want for residue 6\n0. "
		  "Not "
		  "protonated (charge -1) (GLU)\n1. Protonated (charge 0) (GLH)\n\nType a number:";
	const char * g_gln1
		= "Which GLUTAMINE type do you want for residue 12\n0. Not "
		  "protonated (charge 0) (GLN)\n1. Protonated (charge +1) (-)\n\nType a number:";
	const char * g_his1
		= "Which HISTIDINE type do you want for residue 74\n0. H on "
		  "ND1 only (HID)\n1. H on NE2 only (HIE)\n2. H on ND1 and NE2 (HIP)\n3. Coupled to Heme (HIS1)\n\nType a "
		  "number:";
	const char * g_his2
		= "Which HISTIDINE type do you want for residue 81\n0. H on "
		  "ND1 only (HID)\n1. H on NE2 only (HIE)\n2. H on ND1 and NE2 (HIP)\n3. Coupled to Heme (HIS1)\n\nType a "
		  "number:";

	template<size_t SIZE>
	void merge( char ( &b )[ SIZE ], const char * s1, const char * s2 )
	{
		sprintf_s( b, "%s%s", s1, s2 );
	}

} // namespace
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple LYS", "[parseExpectedKwArgument][simple][LYS]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_lys1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::lys, 40 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple ARG", "[parseExpectedKwArgument][simple][ARG]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_arg1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::arg, 31 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple ASP", "[parseExpectedKwArgument][simple][ASP]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_asp1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::asp, 13 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple GLU", "[parseExpectedKwArgument][simple][GLU]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_glu1 );
	CHECK( check_parseExpectedKwArgument(
		b, VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::glu, 6 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple GLN", "[parseExpectedKwArgument][simple][GLN]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_gln1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::gln, 12 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseExpectedKwArgument - simple HIS", "[parseExpectedKwArgument][simple][HIS]" )
{
	char b[ 1000 ];
	merge( b, g_chainB, g_his1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'B', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::his, 74 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parseExpectedKwArgument - last_print_taken - LYS into HIS",
	"[parseExpectedKwArgument][last_print_taken][LYS][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s\n\nSome intense computation\n\nSuch focus, such wow\n\n%s", g_chainB, g_lys1, g_his1 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'B', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::his, 74 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parseExpectedKwArgument - last_print_taken - HIS 1 into HIS 2",
	"[parseExpectedKwArgument][last_print_taken][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s1\n%s", g_chainA, g_his1, g_his2 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::his, 81 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parseExpectedKwArgument - last_print_taken - HIS B into HIS A",
	"[parseExpectedKwArgument][last_print_taken][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s1\n\nSome intense computation\n\nSuch focus, such wow\n\n%s", g_chainA, g_his1, g_his2 );
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'A', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::his, 81 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parseExpectedKwArgument - last_chain_taken",
	"[parseExpectedKwArgument][last_chain_taken]"
)
{
	char b[ 1000 ];
	sprintf_s(
		b,
		"%s%s1\n\nSome intense computation\n\nSuch focus, such wow\n\n%s\nlolol\n%s%s",
		g_chainA,
		g_his1,
		g_his2,
		g_chainB,
		g_lys1
	);
	CHECK( check_parseExpectedKwArgument(
		b,
		VTX::Tool::Mdprep::Gromacs::Pdb2gmxInputId { 'B', VTX::Tool::Mdprep::Gromacs::E_INTERACTIVE_KEYWORD::lys, 40 }
	) );
}

// testing parseOptionNumber

TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - case", "[parseOptionNumber][case][LYS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "LYN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "lyn" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "Not protonated" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "NoT PrOtonATEd" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "PrOtonATEd" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "LYS" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "LyS" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "lys" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - simple LYS", "[parseOptionNumber][simple][LYS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "LYN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_lys1, "LYS" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - simple ARG", "[parseOptionNumber][simple][ARG]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_arg1, "ARG" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - simple GLU", "[parseOptionNumber][simple][GLU]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "GLU" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_glu1, "GLH" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - simple GLN", "[parseOptionNumber][simple][GLN]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "GLN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_gln1, "protonated" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parseOptionNumber - simple HIS", "[parseOptionNumber][simple][HIS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "Not protonated" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "protonated" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "HID" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "HIE" ) == 1 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "2" ) == 2 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "HIP" ) == 2 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "3" ) == 3 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parseOptionNumber( g_his1, "HIS1" ) == 3 );
}
