#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <re2/re2.h>
//
#include <tool/mdprep/backends/gromacs/genion.hpp>
//
#include <tool/mdprep/backends/gromacs/inputs.hpp>
//
#include "tool/mdprep/backends/gromacs/pack.hpp"
#include <tool/mdprep/backends/gromacs/util.hpp>
//
#include "fixture.hpp"

namespace
{
	bool checkAssignmentPattern( const fs::path & p_file, const char * p_varName, const std::string & p_value )
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		REQUIRE( fs::exists( p_file ) );

		auto fileContent = getFileContent( p_file );
		REQUIRE( fileContent.empty() == false );
		RE2 pattern { fmt::format( "{} *= *\"{}\"", p_varName, p_value ) };

		return RE2::PartialMatch( fileContent, pattern );
	}
	bool checkKv( const std::string & p_fileContent, const std::string & p_key, const std::string & p_value )
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;

		REQUIRE( p_fileContent.empty() == false );
		RE2 pattern { fmt::format( "{} *= *{}", p_key, p_value ) };

		return RE2::PartialMatch( p_fileContent, pattern );
	}
} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - createNvtMdp", "[pack][createNvtMdp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createNvtMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.nvt.nsteps	 = 227;
	in.nvt.dt		 = 0.023;
	in.nvt.nstxout	 = 666;
	in.nvt.nstvout	 = 667;
	in.nvt.nstenergy = 668;
	in.nvt.nstlog	 = 999;

	createNvtMdp( outDir, in );

	std::string fileContent = getFileContent( outDir / "nvt.mdp" );
	CHECK( checkKv( fileContent, "nsteps", std::to_string( in.nvt.nsteps ) ) );
	CHECK( checkKv( fileContent, "dt", std::to_string( in.nvt.dt ) ) );
	CHECK( checkKv( fileContent, "nstxout", std::to_string( in.nvt.nstxout ) ) );
	CHECK( checkKv( fileContent, "nstvout", std::to_string( in.nvt.nstvout ) ) );
	CHECK( checkKv( fileContent, "nstenergy", std::to_string( in.nvt.nstenergy ) ) );
	CHECK( checkKv( fileContent, "nstlog", std::to_string( in.nvt.nstlog ) ) );
}
TEST_CASE( "VTX_TOOL_MdPrep - createNptMdp", "[pack][createNptMdp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createNptMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.npt.nsteps	 = 227;
	in.npt.dt		 = 0.023;
	in.npt.nstxout	 = 666;
	in.npt.nstvout	 = 667;
	in.npt.nstenergy = 668;
	in.npt.nstlog	 = 999;

	createNptMdp( outDir, in );

	std::string fileContent = getFileContent( outDir / "npt.mdp" );
	CHECK( checkKv( fileContent, "nsteps", std::to_string( in.npt.nsteps ) ) );
	CHECK( checkKv( fileContent, "dt", std::to_string( in.npt.dt ) ) );
	CHECK( checkKv( fileContent, "nstxout", std::to_string( in.npt.nstxout ) ) );
	CHECK( checkKv( fileContent, "nstvout", std::to_string( in.npt.nstvout ) ) );
	CHECK( checkKv( fileContent, "nstenergy", std::to_string( in.npt.nstenergy ) ) );
	CHECK( checkKv( fileContent, "nstlog", std::to_string( in.npt.nstlog ) ) );
	CHECK( fileContent.find( '\0' ) == std::string::npos );
}
TEST_CASE( "VTX_TOOL_MdPrep - createProdMdp", "[pack][createProdMdp]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createProdMdp";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.prod.nsteps			   = 227;
	in.prod.dt				   = 0.023;
	in.prod.nstxout			   = 666;
	in.prod.nstvout			   = 667;
	in.prod.nstenergy		   = 668;
	in.prod.nstlog			   = 999;
	in.prod.nstxout_compressed = 1212;

	createProdMdp( outDir, in );

	std::string fileContent = getFileContent( outDir / "prod.mdp" );
	CHECK( checkKv( fileContent, "nsteps", std::to_string( in.prod.nsteps ) ) );
	CHECK( checkKv( fileContent, "dt", std::to_string( in.prod.dt ) ) );
	CHECK( checkKv( fileContent, "nstxout", std::to_string( in.prod.nstxout ) ) );
	CHECK( checkKv( fileContent, "nstvout", std::to_string( in.prod.nstvout ) ) );
	CHECK( checkKv( fileContent, "nstenergy", std::to_string( in.prod.nstenergy ) ) );
	CHECK( checkKv( fileContent, "nstlog", std::to_string( in.prod.nstlog ) ) );
	CHECK( checkKv( fileContent, "nstxout-compressed", std::to_string( in.prod.nstxout_compressed ) ) );
	CHECK( fileContent.find( '\0' ) == std::string::npos );
}
TEST_CASE( "VTX_TOOL_MdPrep - createWorkflowPy", "[pack][createWorkflowPy]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "createWorkflowPy";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	MdInstructions in;
	in.fileStem = "1ubq";

	createWorkflowPy( outDir, in );

	CHECK( checkAssignmentPattern( outDir / "runMD.py", "fileStem", in.fileStem ) );
	CHECK( getFileContent( outDir / "runMD.py" ).find( '\0' ) == std::string::npos );
}
TEST_CASE( "VTX_TOOL_MdPrep - pack", "[pack]" )
{
	using namespace VTX::Tool::Mdprep::backends::Gromacs;
	fs::path outDir = VTX::Tool::Mdprep::executableDirectory() / "out" / "pack" / "all";
	if ( fs::exists( outDir ) )
		fs::remove_all( outDir );
	fs::create_directories( outDir );

	CumulativeOuputFiles previousOutputs;

	VTX::test::fill( VTX::test::Grompp2::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Genion::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Grompp::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::TrjConv::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Solvate::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Editconf::g_fileList, previousOutputs );
	VTX::test::fill( VTX::test::Pdb2gmx::g_fileList, previousOutputs );

	MdInstructions in;
	in.fileStem				   = "1ubq";
	in.nvt.nsteps			   = 227;
	in.nvt.dt				   = 0.023;
	in.nvt.nstxout			   = 666;
	in.nvt.nstvout			   = 667;
	in.nvt.nstenergy		   = 668;
	in.nvt.nstlog			   = 999;
	in.npt.nsteps			   = 227;
	in.npt.dt				   = 0.023;
	in.npt.nstxout			   = 666;
	in.npt.nstvout			   = 667;
	in.npt.nstenergy		   = 668;
	in.npt.nstlog			   = 999;
	in.prod.nsteps			   = 227;
	in.prod.dt				   = 0.023;
	in.prod.nstxout			   = 666;
	in.prod.nstvout			   = 667;
	in.prod.nstenergy		   = 668;
	in.prod.nstlog			   = 999;
	in.prod.nstxout_compressed = 1212;

	auto err = pack( outDir, { previousOutputs }, in );

	CHECK( err.error == false );
	CHECK( fs::exists( outDir / "topol.top" ) );
	CHECK( fs::exists( outDir / "em.mdp" ) );
	CHECK( fs::exists( outDir / "em.tpr" ) );
	CHECK( fs::exists( outDir / "runMD.py" ) );
	CHECK( fs::exists( outDir / "nvt.mdp" ) );
	CHECK( fs::exists( outDir / "npt.mdp" ) );
	CHECK( fs::exists( outDir / "prod.mdp" ) );
}
