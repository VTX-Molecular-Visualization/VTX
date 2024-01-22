#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include "tools/mdprep/gromacs.util.hpp"
#include <optional>
#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gromacs
{

	struct gromacs_command_args
	{
		std::vector<std::string>			 arguments;
		std::string							 stdout_;
		std::string							 stderr_;
		std::optional<interactive_arguments> interactive_settings;
		bool								 operator==( const gromacs_command_args & ) const noexcept = default;
	};

	// Blindly execute gromacs with input arguments.
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files as well.
	void submit_gromacs_command( const fs::path & p_gmx_exe, gromacs_command_args & p_args );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
