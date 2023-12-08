#include "tools/mdprep/gromacs/gromacs.h"
// #include "gromacs/gmxpreprocess/pdb2gmx.h"
#include "tools/mdprep/mdprep.hpp" // tmp bad design for the sake of the poc

/*
void submit_gromacs_command( vtx::tool::mdprep::poc_args * args )
{
	gmx::ICommandLineOptionsModule::runAsMain(
		args.i, args.s, "vtx-pdb2gmx", "poc vtx-gromacs", [ & ] { return gmx::pdb2gmxInfo::create(); }
	);
}
*/

extern "C"
{
	void submit_gromacs_command( int argc, char ** argv ) {}
	void simple_function() { double d = 0.; }
}
