#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
//
#include <tools/mdprep/gromacs/genion.hpp>
//
#include <tools/mdprep/gromacs/inputs.hpp>
//
#include "tools/mdprep/gromacs/pack.hpp"
#include <tools/mdprep/gromacs/util.hpp>
//
#include <mdprep/test/fixture.hpp>

namespace
{
	bool checkNsteps( const fs::path & p_file, const std::string & value )
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		REQUIRE( fs::exists( p_file ) );

		auto fileContent = getFileContent( p_file );
		REQUIRE( fileContent.empty() == false );
		RE2 pattern { fmt::format( "nsteps *= *{}", value ) };

		return RE2::PartialMatch( fileContent, pattern );
	}
} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - createNvtMdp", "[pack][createNvtMdp]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createNvtMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.nvtNsteps = 227;

	createNvtMdp( outDir, in );
	CHECK( checkNsteps( outDir / "nvt.mdp", std::to_string( in.nvtNsteps ) ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - createNptMdp", "[pack][createNptMdp]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createNptMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.nptNsteps = 666;

	createNptMdp( outDir, in );
	CHECK( checkNsteps( outDir / "npt.mdp", std::to_string( in.nptNsteps ) ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - createProdMdp", "[pack][createProdMdp]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createProdMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.prodNsteps = 200000000;

	createProdMdp( outDir, in );
	CHECK( checkNsteps( outDir / "prod.mdp", std::to_string( in.prodNsteps ) ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - pack", "[pack]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "pack";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
}
