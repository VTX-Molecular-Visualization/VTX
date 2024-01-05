#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs.impl.hpp>
#include <tools/mdprep/gromacs.util.hpp>

// testing is_waiting_inputs

TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - empty", "[is_waiting_inputs][empty]" )
{
	std::string stdout_ { "" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - exact_pattern", "[is_waiting_inputs][exact_pattern]" )
{
	std::string stdout_ { "\nType a number:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == true );
}
TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - garbage+pattern", "[is_waiting_inputs][garbage+pattern]" )
{
	std::string stdout_ { "lololol BBQ sauce heh\nType a number:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == true );
}
TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - anti-pattern", "[is_waiting_inputs][anti-pattern]" )
{
	std::string stdout_ { "lololol BBQ sauce heh\nType a poney:" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == false );

	stdout_ = "lololol BBQ sauce heh\nType a number:2\nblabla\nsome other writtings\nfancy calculations";
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == false );
}

// testing parse_expected_kw_argument

TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - empty", "[parse_expected_kw_argument][empty]" )
{
	std::string								   stdout_ { "" };
	VTX::Tool::Mdprep::Gromacs::interactive_id kw;

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_expected_kw_argument( stdout_, kw ) == false );
}
namespace
{
	bool check_parse_expected_kw_argument(
		std::string										   stdout_,
		const VTX::Tool::Mdprep::Gromacs::interactive_id & expected_output
	)
	{
		VTX::Tool::Mdprep::Gromacs::interactive_id kw;
		return VTX::Tool::Mdprep::Gromacs::parse_expected_kw_argument( stdout_, kw )
			   && ( kw.chain == expected_output.chain && kw.kw == expected_output.kw && kw.num == expected_output.num );
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
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple LYS", "[parse_expected_kw_argument][simple][LYS]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_lys1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::lys, 40 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple ARG", "[parse_expected_kw_argument][simple][ARG]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_arg1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::arg, 31 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple ASP", "[parse_expected_kw_argument][simple][ASP]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_asp1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::asp, 13 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple GLU", "[parse_expected_kw_argument][simple][GLU]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_glu1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::glu, 6 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple GLN", "[parse_expected_kw_argument][simple][GLN]" )
{
	char b[ 1000 ];
	merge( b, g_chainA, g_gln1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::gln, 12 }
	) );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple HIS", "[parse_expected_kw_argument][simple][HIS]" )
{
	char b[ 1000 ];
	merge( b, g_chainB, g_his1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'B', VTX::Tool::Mdprep::Gromacs::interactive_keyword::his, 74 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_expected_kw_argument - last_print_taken - LYS into HIS",
	"[parse_expected_kw_argument][last_print_taken][LYS][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s\n\nSome intense computation\n\nSuch focus, such wow\n\n%s", g_chainB, g_lys1, g_his1 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'B', VTX::Tool::Mdprep::Gromacs::interactive_keyword::his, 74 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_expected_kw_argument - last_print_taken - HIS 1 into HIS 2",
	"[parse_expected_kw_argument][last_print_taken][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s1\n%s", g_chainA, g_his1, g_his2 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::his, 81 }
	) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - parse_expected_kw_argument - last_print_taken - HIS B into HIS A",
	"[parse_expected_kw_argument][last_print_taken][HIS]"
)
{
	char b[ 1000 ];
	sprintf_s( b, "%s%s1\n\nSome intense computation\n\nSuch focus, such wow\n\n%s", g_chainA, g_his1, g_his2 );
	CHECK( check_parse_expected_kw_argument(
		b, VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::his, 81 }
	) );
}

// testing parse_option_number

TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - case", "[parse_option_number][case][LYS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "LYN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "lyn" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "Not protonated" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "NoT PrOtonATEd" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "PrOtonATEd" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "LYS" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "LyS" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "lys" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - simple LYS", "[parse_option_number][simple][LYS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "LYN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_lys1, "LYS" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - simple ARG", "[parse_option_number][simple][ARG]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_arg1, "ARG" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - simple GLU", "[parse_option_number][simple][GLU]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "GLU" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "protonated" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_glu1, "GLH" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - simple GLN", "[parse_option_number][simple][GLN]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "GLN" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "Not protonated" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_gln1, "protonated" ) == 1 );
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_option_number - simple HIS", "[parse_option_number][simple][HIS]" )
{
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "prout" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "Not protonated" ) == 0xffui8 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "protonated" ) == 0xffui8 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "0" ) == 0 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "HID" ) == 0 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "1" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "HIE" ) == 1 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "2" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "HIP" ) == 1 );

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "3" ) == 1 );
	CHECK( VTX::Tool::Mdprep::Gromacs::parse_option_number( g_his1, "HIS1" ) == 1 );
}
