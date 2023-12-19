#include "tools/mdprep/gromacs/gromacs.hpp"
#include "mdprep/test/fixture.hpp"
#include "tools/mdprep/gromacs.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>

struct io_paths
{
	io_paths( const char * out_dir_name, const char * pdb_code ) :
		in( exec_path / std::format( "data\\{}.pdb", pdb_code ) ),
		out_gro( exec_path / std::format( "{}\\{}.conf.gro", out_dir_name, pdb_code ) ),
		out_topol( exec_path / std::format( "{}\\{}.topol.top", out_dir_name, pdb_code ) ),
		out_posre( exec_path / std::format( "{}\\{}.posre.itp", out_dir_name, pdb_code ) ),
		out_clean( exec_path / std::format( "{}\\{}.clean.pdb", out_dir_name, pdb_code ) ),
		out_index( exec_path / std::format( "{}\\{}.index.ndx", out_dir_name, pdb_code ) )
	{
	}
	const fs::path & exec_path = VTX::Tool::Mdprep::executable_directory();
	fs::path		 in;
	fs::path		 out_gro;
	fs::path		 out_topol;
	fs::path		 out_posre;
	fs::path		 out_clean;
	fs::path		 out_index;
};

void check_pdb( const char * pdb_code )
{
	io_paths paths( pdb_code, pdb_code );

	fs::path out_dir( paths.exec_path / pdb_code );
	if ( fs::is_directory( out_dir ) )
		fs::remove_all( out_dir );
	fs::create_directories( out_dir );

	CHECK_NOFAIL( fs::is_regular_file( paths.in ) );

	VTX::Tool::Mdprep::Gromacs::gromacs_command_args args { {
		"pdb2gmx",
		"-f",
		paths.in.make_preferred().string(),
		"-o",
		paths.out_gro.make_preferred().string(),
		"-p",
		paths.out_topol.make_preferred().string(),
		"-i",
		paths.out_posre.make_preferred().string(),
		"-q",
		paths.out_clean.make_preferred().string(),
		"-n",
		paths.out_index.make_preferred().string(),
		// std::string( "\"" ) + paths.in.make_preferred().string() + "\"",
		//"-o",
		// std::string( "\"" ) + paths.out_gro.make_preferred().string() + "\"",
		//"-p",
		// std::string( "\"" ) + paths.out_topol.make_preferred().string() + "\"",
		//"-i",
		// std::string( "\"" ) + paths.out_posre.make_preferred().string() + "\"",
		//"-q",
		// std::string( "\"" ) + paths.out_clean.make_preferred().string() + "\"",
		//"-n",
		// std::string( "\"" ) + paths.out_index.make_preferred().string() + "\"",
		"-ff",
		"amber03",
		"-water",
		"tip3p",
	} };
	VTX::Tool::Mdprep::Gromacs::declare_ff_directory(
		VTX::Tool::Mdprep::executable_directory() / VTX::Tool::Mdprep::Gromacs::default_ff_directory_relative_path()
	);
	VTX::Tool::Mdprep::Gromacs::submit_gromacs_command( args );

	CHECK( fs::exists( paths.out_gro ) );
	CHECK( fs::exists( paths.out_topol ) );
	CHECK( fs::exists( paths.out_posre ) );
	CHECK( fs::exists( paths.out_clean ) );
	CHECK( fs::exists( paths.out_index ) );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][1ubq]" )
{
	VTX::test::setup_env f;
	check_pdb( "1ubq" );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][1k22]" )
{
	VTX::test::setup_env f;
	check_pdb( "1k22" );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][5j6s]" )
{
	VTX::test::setup_env f;
	check_pdb( "5j6s" );
}

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][5vo4]" )
{
	VTX::test::setup_env f;
	check_pdb( "5vo4" );
}
