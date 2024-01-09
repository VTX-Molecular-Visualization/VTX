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
			in( exec_path / std::format( "data\\{}.pdb", p_pdb_code ) ),
			out_gro( exec_path / std::format( "{}\\{}.conf.gro", p_out_dir_name, p_pdb_code ) ),
			out_topol( exec_path / std::format( "{}\\{}.topol.top", p_out_dir_name, p_pdb_code ) ),
			out_posre( exec_path / std::format( "{}\\{}.posre.itp", p_out_dir_name, p_pdb_code ) ),
			out_clean( exec_path / std::format( "{}\\{}.clean.pdb", p_out_dir_name, p_pdb_code ) ),
			out_index( exec_path / std::format( "{}\\{}.index.ndx", p_out_dir_name, p_pdb_code ) )
		{
		}
		fs::path exec_path = VTX::Tool::Mdprep::executable_directory();
		fs::path in;
		fs::path out_gro;
		fs::path out_topol;
		fs::path out_posre;
		fs::path out_clean;
		fs::path out_index;
	};
	struct test_context
	{
		io_paths										 paths;
		VTX::Tool::Mdprep::Gromacs::gromacs_command_args args;
	};

	bool have_same_content( const fs::path & p_1, const fs::path & p_2 ) noexcept {}

	test_context setup_test_context( const char * p_pdb_code )
	{
		test_context out { { p_pdb_code, p_pdb_code }, {} };

		fs::path out_dir( out.paths.exec_path / p_pdb_code );
		if ( fs::is_directory( out_dir ) )
			fs::remove_all( out_dir );
		fs::create_directories( out_dir );

		CHECK_NOFAIL( fs::is_regular_file( out.paths.in ) );

		out.args = VTX::Tool::Mdprep::Gromacs::gromacs_command_args { {
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
		VTX::Tool::Mdprep::Gromacs::submit_gromacs_command( p_context.args );

		CHECK( fs::exists( p_context.paths.out_gro ) );
		CHECK( fs::exists( p_context.paths.out_topol ) );
		CHECK( fs::exists( p_context.paths.out_posre ) );
		CHECK( fs::exists( p_context.paths.out_clean ) );
		CHECK( fs::exists( p_context.paths.out_index ) );
	}

} // namespace

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][1ubq]" )
{
	VTX::test::setup_env f;
	check_pdb( setup_test_context( "1ubq" ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 8hu4", "[submit_gromacs_command][pdb2gmx][8hu4]" )
{
	VTX::test::setup_env f;
	check_pdb( setup_test_context( "8hu4.nolig" ) );
}

// TODO : check with other forcefields as well
