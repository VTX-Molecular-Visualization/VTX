#include "tools/mdprep/mdprep.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"
#include <qprocess.h>

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		// We need gromacs to see top param data files. One way to achieve this is to set an env var to the path of the
		// folder containing those data
		// QProcess			process;
		// QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
		// env.insert( "GMXLIB", "D:\\dev\\vtx\\lib\\tool\\tools\\mdprep\\external\\gromacs\\share\\top" );
		// process.setProcessEnvironment( env );
		// return;
		gmx::ICommandLineOptionsModule::runAsMain(
			args.i, args.s, "ctx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}

} // namespace vtx::tool::mdprep
