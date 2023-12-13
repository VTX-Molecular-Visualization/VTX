#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS_ARGS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS_ARGS__

#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gromacs
{
	struct gromacs_command_args
	{
		std::vector<std::string> arguments;
	};

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
