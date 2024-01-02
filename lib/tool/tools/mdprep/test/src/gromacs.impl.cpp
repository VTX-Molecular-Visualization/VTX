#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
//
#include <tools/mdprep/gromacs.impl.hpp>

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

	std::string stdout_ { "lololol BBQ sauce heh\nType a number:2\nblabla\nsome other writtings\nfancy calculations" };
	CHECK( VTX::Tool::Mdprep::Gromacs::is_waiting_inputs( stdout_ ) == false );
}
