#include "tools/mdprep/mdprep.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[poc]" )
{
	std::vector<std::string> args {
		//"\"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\build\\Debug\\vtx_tool_mdprep_test.exe\"",
		"pdb2gmx",
		"-f",
		"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\data\\1ubq.pdb",
		//"-o",
		//"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\data\\1ubq.gro",
		"-ff",
		"amber03",
	};
	std::vector<char *> cmd( args.size() );
	for ( int i = 0; i < args.size(); i++ )
	{
		cmd[ i ] = args[ i ].data();
	}

	vtx::tool::mdprep::poc_args poc_args { .i = static_cast<int>( args.size() ), .s = cmd.data() };
	vtx::tool::mdprep::poc_execute_cmd( poc_args );
}
