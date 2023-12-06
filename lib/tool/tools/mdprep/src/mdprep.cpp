#include "tools/mdprep/mdprep.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"
#include <filesystem>
#include <qapplication.h>

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		// We need gromacs to see top param data files. One way to achieve this is to set an env var to the path of the
		// folder containing those data
		{
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );

			std::filesystem::path path( path_str );
			path /= "data";
			path /= "tool";
			path /= "tools";
			path /= "mdprep";
			path /= "gromacs";
			path /= "top";
			path_str = path.string();
			QByteArrayView env_arg( path_str.begin().operator->(), path_str.end().operator->() );
			qputenv( "GMXLIB", env_arg );
		}

		gmx::ICommandLineOptionsModule::runAsMain(
			args.i, args.s, "ctx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}

} // namespace vtx::tool::mdprep
