#pragma once

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

	const fs::path		 g_logPath( "ChildLog.log" );
	inline std::ofstream log()
	{
		std::ofstream f( g_logPath, std::ios::app );
		f << boost::process::current_pid() << " - ";
		return f;
	}
} // namespace pdb100
