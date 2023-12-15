#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS_ARGS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMACS_ARGS__

#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gromacs
{
	struct gromacs_command_args
	{
		std::vector<std::string> arguments;
		std::string				 out;
	};
	inline bool operator==( const gromacs_command_args & l, const gromacs_command_args & r ) noexcept
	{
		return l.arguments == r.arguments;
	}

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
