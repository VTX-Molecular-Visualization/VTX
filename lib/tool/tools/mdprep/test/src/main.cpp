#include "tools/mdprep/mdprep.hpp"
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <util/logger.hpp>
#include <vector>

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[poc]" )
{
	VTX::Util::Logger::get().init();
	VTX::App::VTXApp::get().start( {} );
	// assumes share/top content is exploded into the exec dir for now
	std::vector<std::string> args {
		"pdb2gmx",
		"-f",
		"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\data\\1ubq.pdb",
		//"-o",
		//"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\data\\1ubq.gro",
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

		vtx::tool::mdprep::poc_args poc_args { .i = static_cast<int>( args.size() ), .s = cmd };
		vtx::tool::mdprep::poc_execute_cmd( poc_args );
	}
	bool b = true;
}
