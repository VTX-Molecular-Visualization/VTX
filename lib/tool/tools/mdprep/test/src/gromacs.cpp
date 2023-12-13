#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/gromacs.util.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
struct io_paths
{
	io_paths( const char * pdb_code ) :
		in( exec_path / std::format( "data\\{}.pdb", pdb_code ) ),
		out_gro( exec_path / std::format( "data\\{}.conf.gro", pdb_code ) ),
		out_topol( exec_path / std::format( "data\\{}.topol.top", pdb_code ) ),
		out_posre( exec_path / std::format( "data\\{}.posre.itp", pdb_code ) ),
		out_clean( exec_path / std::format( "data\\{}.clean.pdb", pdb_code ) ),
		out_index( exec_path / std::format( "data\\{}.index.ndx", pdb_code ) )
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

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[submit_gromacs_command][pdb2gmx]" )
{
	io_paths paths( "1ubq" );

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
		"tip3p",
		"-water",
	} };
}
