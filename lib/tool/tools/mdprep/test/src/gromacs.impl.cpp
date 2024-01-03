#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs.impl.hpp>
#include <tools/mdprep/gromacs.util.hpp>

TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - empty", "[is_waiting_inputs][empty]" )
{
	std::string stdout_ { "" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == false );
}
TEST_CASE( "VTX_TOOL_MdPrep - is_waiting_inputs - exact_pattern", "[is_waiting_inputs][exact_pattern]" )
{
	std::string stdout_ { "Type a number:" };
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

TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - empty", "[parse_expected_kw_argument][empty]" )
{
	std::string								   stdout_ { "" };
	VTX::Tool::Mdprep::Gromacs::interactive_id kw;

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_expected_kw_argument( stdout_, kw ) == false );
}
bool check_parse_expected_kw_argument(std::string stdout_, const VTX::Tool::Mdprep::Gromacs::interactive_id& expected_output)
{
	VTX::Tool::Mdprep::Gromacs::interactive_id kw;
	return VTX::Tool::Mdprep::Gromacs::parse_expected_kw_argument( stdout_, kw ) && (kw.chain == expected_output.chain && kw.kw == expected_output.kw && kw.num == expected_output.num);
}

TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple LYS", "[parse_expected_kw_argument][simple][LYS]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich LYSINE type do you want for residue 40\n0. Not protonated (charge 0) (LYN)\n1. Protonated (charge +1) (LYS)\n\nType a number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::lys, 40 }
	));
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple ARG", "[parse_expected_kw_argument][simple][ARG]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich ARGININE type do you want for residue 31\n0. Not protonated (charge 0) (-)\n1. Protonated (charge +1) (ARG)\n\nType a number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::arg, 31 }
	));
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple ASP", "[parse_expected_kw_argument][simple][ASP]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich ASPARTIC ACID type do you want for residue 13\n0. Not protonated (charge -1) (ASP)\n1. Protonated (charge 0) (ASH)\n\nType a number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::asp, 13 }
	));
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple GLU", "[parse_expected_kw_argument][simple][GLU]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich GLUTAMIC ACID type do you want for residue 6\n0. Not protonated (charge -1) (GLU)\n1. Protonated (charge 0) (GLH)\n\nType a number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::glu, 6 }
	));
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple GLN", "[parse_expected_kw_argument][simple][GLN]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich GLUTAMINE type do you want for residue 12\n0. Not protonated (charge 0) (GLN)\n1. Protonated (charge +1) (-)\n\nType a number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'A', VTX::Tool::Mdprep::Gromacs::interactive_keyword::gln, 12 }
	));
}
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple HIS", "[parse_expected_kw_argument][simple][HIS]" )
{
	CHECK(check_parse_expected_kw_argument(
		"Processing chain 1 'B' (6457 atoms, 827 residues)\nWhich HISTIDINE type do you want for residue 74\n0. H on "
		"ND1 only (HID)\n1. H on NE2 only (HIE)\n2. H on ND1 and NE2 (HIP)\n3. Coupled to Heme (HIS1)\n\nType a "
		"number:",
		VTX::Tool::Mdprep::Gromacs::interactive_id { 'B', VTX::Tool::Mdprep::Gromacs::interactive_keyword::his, 74 }
	));
}
