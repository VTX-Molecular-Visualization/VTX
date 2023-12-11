#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/gromacs/gromacs.hpp"
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <algorithm>
#include <util/logger.hpp>
#include <vector>

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[poc]" )
{
	VTX::Util::Logger::get().init();

	// assumes share/top content is exploded into the exec dir for now
	std::vector<std::string> args {
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
	{
		char ** cmd
			= reinterpret_cast<char **>( std::malloc( sizeof( char * ) * args.size() ) ); // gromacs will free it ...
		for ( int i = 0; i < args.size(); i++ )
		{
			args[ i ] += '\0';
			cmd[ i ] = &args.at( i ).at( 0 );
		}

		VTX::Tool::Mdprep::gromacs_command_args args { std::move(args) };
		VTX::Tool::Mdprep::poc_execute_cmd( args );
	}
	bool b = true;
}
