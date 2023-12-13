#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/gromacs.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <qapplication.h>

struct io_paths
{
	io_paths( const char * out_dir_name, const char * pdb_code ) :
		in( exec_path / std::format( "data\\{}.pdb", pdb_code ) ),
		out_gro( exec_path / std::format( "data\\{}\\{}.conf.gro", out_dir_name, pdb_code ) ),
		out_topol( exec_path / std::format( "data\\{}\\{}.topol.top", out_dir_name, pdb_code ) ),
		out_posre( exec_path / std::format( "data\\{}\\{}.posre.itp", out_dir_name, pdb_code ) ),
		out_clean( exec_path / std::format( "data\\{}\\{}.clean.pdb", out_dir_name, pdb_code ) ),
		out_index( exec_path / std::format( "data\\{}\\{}.index.ndx", out_dir_name, pdb_code ) )
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

TEST_CASE( "VTX_TOOL_MdPrep - gmx pdb2gmx 1ubq", "[submit_gromacs_command][pdb2gmx][1ubq]" )
{
	int				 argc = 0;
	QCoreApplication app( argc, nullptr );
	QString			 qpath = app.applicationDirPath();

	const char * out_dir_name = "out";
	io_paths	 paths( out_dir_name, "1ubq" );

	fs::path out_dir( paths.exec_path / out_dir_name );
	if ( fs::is_directory( out_dir ) )
		fs::remove_all( out_dir );
	fs::create_directories( out_dir );

	CHECK_NOFAIL( fs::is_regular_file( paths.in ) );

	VTX::Tool::Mdprep::Gromacs::gromacs_command_args args { {
		"pdb2gmx",
		"-f",
		paths.in.string(),
		"-o",
		paths.out_gro.string(),
		"-p",
		paths.out_topol.string(),
		"-i",
		paths.out_posre.string(),
		"-q",
		paths.out_clean.string(),
		"-n",
		paths.out_index.string(),
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
