#include "tools/mdprep/gromacs/gromacs.hpp"
#include "tools/mdprep/mdprep.hpp"
#include <algorithm>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <util/logger.hpp>
#include <vector>

/*
TEST_CASE( "VTX_TOOL_MdPrep - Test", "[poc]" )
{
	VTX::Util::Logger::get().init();

	// assumes share/top content is exploded into the exec dir for now
	std::vector<std::string> arguments_vector {
		"pdb2gmx",
		"-f",
		"data\\1ubq.pdb",
		"-o",
		"out\\1ubq.conf.gro",
		"-p",
		"out\\1ubq.topol.top",
		"-i",
		"out\\1ubq.posre.itp",
		"-q",
		"out\\1ubq.clean.pdb",
		"-n",
		"out\\1ubq.index.ndx",
		"-ff",
		"amber03",
		"-water",
		"tip3p",
	};
	VTX::Tool::Mdprep::gromacs_command_args args { std::move( arguments_vector ) };
	VTX::Tool::Mdprep::poc_execute_cmd( args );
}
*/
