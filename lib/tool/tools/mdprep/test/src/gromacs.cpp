#include "tools/mdprep/gromacs.hpp"
#include "mdprep/test/fixture.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>

namespace
{

	struct io_paths
	{
		io_paths( const char * p_out_dir_name, const char * p_pdb_code ) :
			in( exec_path / std::format( "data\\{}.pdb", p_pdb_code ) ), out_dir( exec_path / "out" / p_pdb_code ),
			out_gro( exec_path / std::format( "{}.conf.gro", p_out_dir_name, p_pdb_code ) ),
			out_topol( out_dir / std::format( "{}.topol.top", p_out_dir_name, p_pdb_code ) ),
			out_posre( out_dir / std::format( "{}.posre.itp", p_out_dir_name, p_pdb_code ) ),
			out_clean( out_dir / std::format( "{}.clean.pdb", p_out_dir_name, p_pdb_code ) ),
			out_index( out_dir / std::format( "{}.index.ndx", p_out_dir_name, p_pdb_code ) )
		{
		}
		fs::path exec_path = VTX::Tool::Mdprep::executable_directory();
		fs::path in;
		fs::path out_dir;
		fs::path out_gro;
		fs::path out_topol;
		fs::path out_posre;
		fs::path out_clean;
		fs::path out_index;
	};
	struct test_context
	{
		io_paths									   paths;
		VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs args;
	};

	bool have_same_content( const fs::path & p_1, const fs::path & p_2 ) noexcept {}

	bool check_if_exists(
		const fs::path & directory,
		const char *	 starting_pattern,
		const char *	 ending_pattern
	) noexcept
	{
		for ( auto file : fs::directory_iterator( directory ) )
		{
			std::string filename = file.path().filename().string();
			if ( filename.starts_with( starting_pattern ) && filename.ends_with( ending_pattern ) )
				return true;
		}
		return false;
	}
	bool check_file_as_pattern( const fs::path & file_patterned ) noexcept
	{
		fs::path	dir		 = file_patterned.parent_path();
		std::string filename = file_patterned.stem().string().data();
		std::string ext		 = file_patterned.extension().string().data();
		return check_if_exists( dir, filename.data(), ext.data() );
	}

	test_context setup_test_context( const char * p_pdb_code )
	{
		test_context out { { p_pdb_code, p_pdb_code }, {} };

		fs::path & out_dir = out.paths.out_dir;
		if ( fs::is_directory( out_dir ) )
			fs::remove_all( out_dir );
		fs::create_directories( out_dir );

		CHECK_NOFAIL( fs::is_regular_file( out.paths.in ) );

		out.args = VTX::Tool::Mdprep::Gromacs::GromacsCommandArgs { {
			"pdb2gmx",
			"-f",
			out.paths.in.make_preferred().string(),
			"-o",
			out.paths.out_gro.make_preferred().string(),
			"-p",
			out.paths.out_topol.make_preferred().string(),
			"-i",
			out.paths.out_posre.make_preferred().string(),
			"-q",
			out.paths.out_clean.make_preferred().string(),
			"-n",
			out.paths.out_index.make_preferred().string(),
			"-ff",
			"amber03",
			"-water",
			"tip3p",
		} };
		VTX::Tool::Mdprep::Gromacs::declare_ff_directory(
			VTX::Tool::Mdprep::executable_directory() / VTX::Tool::Mdprep::Gromacs::default_ff_directory_relative_path()
		);

		return out;
	}

	void check_pdb( test_context p_context )
	{
		VTX::Tool::Mdprep::Gromacs::submitGromacsCommand(
			VTX::Tool::Mdprep::executable_directory() / VTX::Tool::Mdprep::Gromacs::default_gmx_binary_relative_path(),
			p_context.args
		);
		// for topol and posre, gromacs do not necessarily output a file with the exact name, but divide chains and ions
		// into multiple files. So we need to check its pattern for us to be sure everything worked.
		CHECK( fs::exists( p_context.paths.out_gro ) );
		CHECK( check_file_as_pattern( p_context.paths.out_topol ) );
		CHECK( check_file_as_pattern( p_context.paths.out_posre ) );
		CHECK( fs::exists( p_context.paths.out_clean ) );
		CHECK( fs::exists( p_context.paths.out_index ) );
	}

} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submitGromacsCommand][pdb2gmx][1ubq]" )
{
	VTX::test::setup_env f;
	check_pdb( setup_test_context( "1ubq" ) );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq - A LYN6",
	"[submitGromacsCommand][pdb2gmx][1ubq][interactive][full][lys]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	VTX::test::setup_env f;
	auto				 context_data = setup_test_context( "1ubq" );
	context_data.args.interactive_settings.emplace();
	context_data.args.interactive_settings->kw_v.emplace( InteractiveId { 'A', interactive_keyword::lys, 6 }, "LYN" );
	check_pdb( context_data );
}
TEST_CASE(
	"VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq - A LYN6",
	"[submitGromacsCommand][pdb2gmx][1ubq][interactive][default]"
)
{
	using namespace VTX::Tool::Mdprep::Gromacs;
	VTX::test::setup_env f;
	auto				 context_data = setup_test_context( "1ubq" );
	context_data.args.interactive_settings.emplace();
	context_data.args.interactive_settings->kw_v.emplace( InteractiveId { 'A', interactive_keyword::lys, 6 }, "LYN" );
	check_pdb( context_data );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 8hu4", "[submitGromacsCommand][pdb2gmx][8hu4][slow]" )
{
	VTX::test::setup_env f;
	check_pdb( setup_test_context( "8hu4.nolig" ) );
}

// TODO : check with interactive stuff
// TODO : check with other forcefields as well
