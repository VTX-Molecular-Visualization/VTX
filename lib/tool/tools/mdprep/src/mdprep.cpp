#include "tools/mdprep/mdprep.hpp"
#include "gromacs/gmxpreprocess/pdb2gmx.h"

namespace vtx::tool::mdprep
{
	void poc_execute_cmd( poc_args & args ) noexcept
	{
		gmx::ICommandLineOptionsModule::runAsMain(
			args.i, args.s, "ctx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
		);
	}

} // namespace vtx::tool::mdprep
