#include "tool/mdprep/backends/gromacs/pdb2gmx.hpp"
//
#include "fixture.hpp"
#include "tool/mdprep/backends/gromacs/job.hpp"
#include "tool/mdprep/backends/gromacs/pdb2gmx.hpp"
#include "tool/mdprep/backends/gromacs/util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

namespace
{

	struct IoPaths
	{
		IoPaths( const char * p_out_dir_name, const char * p_pdb_code ) :
			in( execPath / "data" / fmt::format( "{}.pdb", p_pdb_code ) ), outDir( execPath / "out" / p_pdb_code ),
			outGro( outDir / fmt::format( "{}.conf.gro", p_out_dir_name, p_pdb_code ) ),
			outTopol( outDir / fmt::format( "{}.topol.top", p_out_dir_name, p_pdb_code ) ),
			outPosre( outDir / fmt::format( "{}.posre.itp", p_out_dir_name, p_pdb_code ) ),
			outClean( outDir / fmt::format( "{}.clean.pdb", p_out_dir_name, p_pdb_code ) ),
			outIndex( outDir / fmt::format( "{}.index.ndx", p_out_dir_name, p_pdb_code ) )
		{
			execPath.make_preferred();
			in.make_preferred();
			outDir.make_preferred();
			outGro.make_preferred();
			outTopol.make_preferred();
			outPosre.make_preferred();
			outClean.make_preferred();
			outIndex.make_preferred();
		}

		FilePath execPath = VTX::App::SESSION().getApplicationDir();
		FilePath in;
		FilePath outDir;
		FilePath outGro;
		FilePath outTopol;
		FilePath outPosre;
		FilePath outClean;
		FilePath outIndex;
	};

	struct TestContext
	{
		IoPaths												 paths;
		VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData args;
	};

	bool checkIfExists(
		const FilePath & p_directory,
		const char *	 p_startingPattern,
		const char *	 p_endingPattern
	) noexcept
	{
		for ( auto file : fs::directory_iterator( p_directory ) )
		{
			std::string filename = file.path().filename().string();
			if ( filename.starts_with( p_startingPattern ) && filename.ends_with( p_endingPattern ) )
			{
				return true;
			}
		}
		return false;
	}

	bool check_file_as_pattern( const FilePath & p_filePatterned ) noexcept
	{
		FilePath	dir		 = p_filePatterned.parent_path();
		std::string filename = p_filePatterned.stem().string().data();
		std::string ext		 = p_filePatterned.extension().string().data();
		return checkIfExists( dir, filename.data(), ext.data() );
	}

	TestContext setupTestContext( const char * p_pdbCode )
	{
		TestContext out { { p_pdbCode, p_pdbCode }, {} };

		FilePath & outDir = out.paths.outDir;
		if ( fs::is_directory( outDir ) )
		{
			fs::remove_all( outDir );
		}
		fs::create_directories( outDir );

		CHECK_NOFAIL( fs::is_regular_file( out.paths.in ) );

		out.args = VTX::Tool::Mdprep::backends::Gromacs::GromacsJobData { {
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
		VTX::Tool::Mdprep::backends::Gromacs::declareFfDirectory(
			VTX::Tool::Mdprep::backends::Gromacs::defaultFfDirectoryPath()
		);

		return out;
	}

	void check_pdb( TestContext p_context )
	{
		FilePath full_gmx_exe_path = VTX::Tool::Mdprep::backends::Gromacs::defaultGmxBinaryPath();
		VTX::Tool::Mdprep::backends::Gromacs::submitGromacsJob( full_gmx_exe_path, p_context.args );
		// for topol and posre, gromacs do not necessarily output a file with the exact name, but divide chains and ions
		// into multiple files. So we need to check its pattern for us to be sure everything worked.
		auto channels = ( p_context.args.channelsLocker.open() );
		INFO( channels->stdout_ );
		INFO( channels->stderr_ );
		CHECK( fs::exists( p_context.paths.outGro ) );
		CHECK( check_file_as_pattern( p_context.paths.outTopol ) );
		CHECK( check_file_as_pattern( p_context.paths.outPosre ) );
		CHECK( fs::exists( p_context.paths.outClean ) );
		CHECK( fs::exists( p_context.paths.outIndex ) );
	}

} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submitGromacsJob][pdb2gmx][1ubq]" )
{
	VTX::test::setup_env f;
	check_pdb( setupTestContext( "1ubq" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 8hu4", "[submitGromacsJob][pdb2gmx][8hu4][slow]" )
{
	VTX::test::setup_env f;
	check_pdb( setupTestContext( "8hu4.nolig" ) );
}

// TODO : check with other forcefields as well
