#include "tools/mdprep/gromacs/gromacs.hpp"
#include "mdprep/test/fixture.hpp"
#include "tools/mdprep/gromacs/gromacs.util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>

namespace
{

	struct IoPaths
	{
		IoPaths( const char * p_out_dir_name, const char * p_pdb_code ) :
			in( execPath / std::format( "data\\{}.pdb", p_pdb_code ) ), outDir( execPath / "out" / p_pdb_code ),
			outGro( outDir / std::format( "{}.conf.gro", p_out_dir_name, p_pdb_code ) ),
			outTopol( outDir / std::format( "{}.topol.top", p_out_dir_name, p_pdb_code ) ),
			outPosre( outDir / std::format( "{}.posre.itp", p_out_dir_name, p_pdb_code ) ),
			outClean( outDir / std::format( "{}.clean.pdb", p_out_dir_name, p_pdb_code ) ),
			outIndex( outDir / std::format( "{}.index.ndx", p_out_dir_name, p_pdb_code ) )
		{
		}
		fs::path execPath = VTX::Tool::Mdprep::executableDirectory();
		fs::path in;
		fs::path outDir;
		fs::path outGro;
		fs::path outTopol;
		fs::path outPosre;
		fs::path outClean;
		fs::path outIndex;
	};
	struct TestContext
	{
		IoPaths										   paths;
		VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;
	};

	bool haveSameContent( const fs::path & p_1, const fs::path & p_2 ) noexcept {}

	bool checkIfExists(
		const fs::path & p_directory,
		const char *	 p_startingPattern,
		const char *	 p_endingPattern
	) noexcept
	{
		for ( auto file : fs::directory_iterator( p_directory ) )
		{
			std::string filename = file.path().filename().string();
			if ( filename.starts_with( p_startingPattern ) && filename.ends_with( p_endingPattern ) )
				return true;
		}
		return false;
	}
	bool check_file_as_pattern( const fs::path & p_filePatterned ) noexcept
	{
		fs::path	dir		 = p_filePatterned.parent_path();
		std::string filename = p_filePatterned.stem().string().data();
		std::string ext		 = p_filePatterned.extension().string().data();
		return checkIfExists( dir, filename.data(), ext.data() );
	}

	TestContext setupTestContext( const char * p_pdbCode )
	{
		TestContext out { { p_pdbCode, p_pdbCode }, {} };

		fs::path & outDir = out.paths.outDir;
		if ( fs::is_directory( outDir ) )
			fs::remove_all( outDir );
		fs::create_directories( outDir );

		CHECK_NOFAIL( fs::is_regular_file( out.paths.in ) );

		out.args = VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs { {
			"pdb2gmx",
			"-f",
			out.paths.in.make_preferred().string(),
			"-o",
			out.paths.outGro.make_preferred().string(),
			"-p",
			out.paths.outTopol.make_preferred().string(),
			"-i",
			out.paths.outPosre.make_preferred().string(),
			"-q",
			out.paths.outClean.make_preferred().string(),
			"-n",
			out.paths.outIndex.make_preferred().string(),
			"-ff",
			"amber03",
			"-water",
			"tip3p",
		} };
		VTX::Tool::Mdprep::Gromacs::declareFfDirectory(
			VTX::Tool::Mdprep::executableDirectory() / VTX::Tool::Mdprep::Gromacs::defaultFfDirectoryRelativePath()
		);

		return out;
	}

	void check_pdb( TestContext p_context )
	{
		fs::path full_gmx_exe_path
			= VTX::Tool::Mdprep::executableDirectory() / VTX::Tool::Mdprep::Gromacs::defaultGmxBinaryRelativePath();
		VTX::Tool::Mdprep::Gromacs::submitGromacsCommand( full_gmx_exe_path, p_context.args );
		// for topol and posre, gromacs do not necessarily output a file with the exact name, but divide chains and ions
		// into multiple files. So we need to check its pattern for us to be sure everything worked.
		CHECK( fs::exists( p_context.paths.outGro ) );
		CHECK( check_file_as_pattern( p_context.paths.outTopol ) );
		CHECK( check_file_as_pattern( p_context.paths.outPosre ) );
		CHECK( fs::exists( p_context.paths.outClean ) );
		CHECK( fs::exists( p_context.paths.outIndex ) );
	}

} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submitGromacsCommand][pdb2gmx][1ubq]" )
{
	VTX::test::setup_env f;
	check_pdb( setupTestContext( "1ubq" ) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq - A LYN6",
	"[submitGromacsCommand][pdb2gmx][1ubq][interactive][full][lys]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	VTX::test::setup_env f;
	auto				 contextData = setupTestContext( "1ubq" );

	contextData.args.arguments.push_back( "-lys" );
	contextData.args.interactiveSettings.emplace();
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 6 }, "0" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 11 }, "1" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 27 }, "1" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 29 }, "0" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 33 }, "1" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 48 }, "1" );
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 63 }, "0" );
	check_pdb( contextData );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq - A LYN6",
	"[submitGromacsCommand][pdb2gmx][1ubq][interactive][default]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	VTX::test::setup_env f;
	auto				 contextData = setupTestContext( "1ubq" );

	contextData.args.arguments.push_back( "-lys" );
	contextData.args.interactiveSettings.emplace();
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'A', E_INTERACTIVE_KEYWORD::lys, 6 }, "0" );
	check_pdb( contextData );
}
TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 8hu4 - B GLN62", "[submitGromacsCommand][pdb2gmx][8hu4][interactive][slow]" )
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	VTX::test::setup_env f;
	auto				 contextData = setupTestContext( "8hu4.nolig" );

	contextData.args.arguments.push_back( "-lys" );
	contextData.args.interactiveSettings.emplace();
	contextData.args.interactiveSettings->kwValue.emplace(
		InteractiveId { 'A', E_INTERACTIVE_KEYWORD::gln, 289 }, "1"
	);
	contextData.args.interactiveSettings->kwValue.emplace( InteractiveId { 'B', E_INTERACTIVE_KEYWORD::gln, 62 }, "1" );
	check_pdb( contextData );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 8hu4", "[submitGromacsCommand][pdb2gmx][8hu4][slow]" )
{
	VTX::test::setup_env f;
	check_pdb( setupTestContext( "8hu4.nolig" ) );
}

// TODO : check with interactive stuff
// TODO : check with other forcefields as well
