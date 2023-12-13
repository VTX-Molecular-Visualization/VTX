#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS__

#include "tools/mdprep/gromacs/base.hpp"

namespace VTX::Tool::Mdprep::Gromacs
{
	struct gromacs_command_args;

} // namespace VTX::Tool::Mdprep::Gromacs

extern "C"
{
	LIB_EXPORT void submit_gromacs_command( VTX::Tool::Mdprep::Gromacs::gromacs_command_args & );
}

#endif
