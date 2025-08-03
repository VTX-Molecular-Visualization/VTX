#pragma once

#include <fmt/format.h>
#include <fstream>
#include <vtx/secondary_structure/shared/interprocess.hpp>
//
#include <boost/process/v2/pid.hpp>
namespace pdb100
{
	/**
	 * @brief Fetch and tests systems
	 */
	void testSystems();

	inline std::ofstream log()
	{
		fs::path dirName = "childlogs";
		if ( not fs::exists( dirName ) )
			fs::create_directories( dirName );

		fs::path logPath( dirName / fmt::format( "ChildLog-{}.log", boost::process::current_pid() ) );

		std::ofstream f( logPath, std::ios::app );
		return f;
	}
} // namespace pdb100
