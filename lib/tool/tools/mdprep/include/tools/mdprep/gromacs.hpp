#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gromacs
{

	struct gromacs_command_args
	{
		std::vector<std::string> arguments;
		std::string				 stdout_;
		std::string				 stderr_;
	};
	inline bool operator==(
		const VTX::Tool::Mdprep::Gromacs::gromacs_command_args & l,
		const VTX::Tool::Mdprep::Gromacs::gromacs_command_args & r
	) noexcept
	{
		return l.arguments == r.arguments;
	}

	// Blindly execute gromacs with input arguments.
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files as well.
	void submit_gromacs_command( gromacs_command_args & );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
