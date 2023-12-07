#include "tools/mdprep/gromacs/gromacs.h"
#include "gromacs/gmxpreprocess/pdb2gmx.h"
#include "tools/mdprep/mdprep.hpp"

namespace vtx::tool::mdprep
{
	void submit_gromacs_command( poc_args & args )
	{
		gmx::ICommandLineOptionsModule::runAsMain(
			args.i, args.s, "vtx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}
} // namespace vtx::tool::mdprep
