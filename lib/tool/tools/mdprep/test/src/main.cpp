#include "tools/mdprep/mdprep.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE( "VTX_TOOL_MdPrep - Test", "[poc]" )
{
	std::vector<std::string> args {
		"\"D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\test\\build\\Debug\\vtx_tool_mdprep_test.exe\"",
		"pdb2gmx",
		"\"data\\1ubq.pdb\"",
		"-ff",
		"amber03",
	};
	char ** cmd = reinterpret_cast<char **>( std::malloc( args.size() * sizeof( char * ) ) );

	for ( int i = 0; i < args.size(); i++ )
	{
		cmd[ i ] = args[ i ].data();
	}

	vtx::tool::mdprep::poc_args poc_args { .i = static_cast<int>( args.size() ), .s = cmd };
	vtx::tool::mdprep::poc_execute_cmd( poc_args );
}
