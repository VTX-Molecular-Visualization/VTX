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
	bool checkAssignmentPattern( const fs::path & p_file, const char * p_varName, const std::string & p_value )
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		REQUIRE( fs::exists( p_file ) );

		auto fileContent = getFileContent( p_file );
		REQUIRE( fileContent.empty() == false );
		RE2 pattern { fmt::format( "{} *= *\"{}\"", p_varName, p_value ) };

		return RE2::PartialMatch( fileContent, pattern );
	}
	bool checkNsteps( const fs::path & p_file, const std::string & p_value )
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		REQUIRE( fs::exists( p_file ) );

		auto fileContent = getFileContent( p_file );
		REQUIRE( fileContent.empty() == false );
		RE2 pattern { fmt::format( "nsteps *= *{}", p_value ) };

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
TEST_CASE( "VTX_TOOL_MdPrep - createWorkflowPy", "[pack][createWorkflowPy]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createWorkflowPy";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.fileStem = "1ubq";

	createWorkflowPy( outDir, in );
	CHECK( checkAssignmentPattern( outDir / "runMD.py", "fileStem", in.fileStem ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - pack", "[pack]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "pack";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	std::vector<const std::string *> previousOutputs;

	VTX::test::fill( VTX::test::Grompp2::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Genion::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	MdInstructions in;
	in.fileStem	  = "1ubq";
	in.nvtNsteps  = 100;
	in.nptNsteps  = 1000;
	in.prodNsteps = 100000;

	auto err = pack( outDir, { previousOutputs }, in );

	CHECK( err.error == false );
	CHECK( fs::exists( outDir / "em.mdp" ) );
	CHECK( fs::exists( outDir / "em.tpr" ) );
	CHECK( fs::exists( outDir / "runMD.py" ) );
	CHECK( fs::exists( outDir / "nvt.mdp" ) );
	CHECK( fs::exists( outDir / "npt.mdp" ) );
	CHECK( fs::exists( outDir / "prod.mdp" ) );
}
