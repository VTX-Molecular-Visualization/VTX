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
TEST_CASE( "VTX_TOOL_MdPrep - parse_expected_kw_argument - simple HIS", "[parse_expected_kw_argument][simple][HIS]" )
{
	std::string stdout_ {
		"Processing chain 1 'A' (6457 atoms, 827 residues)\nWhich HISTIDINE type do you want for residue 74\n0. H on "
		"ND1 only (HID)\n1. H on NE2 only (HIE)\n2. H on ND1 and NE2 (HIP)\n3. Coupled to Heme (HIS1)\n\nType a number:"
	};
	VTX::Tool::Mdprep::Gromacs::interactive_id kw;

	CHECK( VTX::Tool::Mdprep::Gromacs::parse_expected_kw_argument( stdout_, kw ) == true );
	CHECK( kw.kw == VTX::Tool::Mdprep::Gromacs::interactive_keyword::his );
	CHECK( kw.num == 74 );
}
