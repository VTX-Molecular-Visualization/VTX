#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#include "tools/mdprep/gromacs/gromacs.args.hpp"

namespace VTX::Tool::Mdprep::Gromacs
{
	// Blindly execute gromacs with input arguments.
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files as well.
	void submit_gromacs_command( gromacs_command_args & );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
