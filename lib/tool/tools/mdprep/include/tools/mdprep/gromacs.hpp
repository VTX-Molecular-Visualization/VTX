#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include "tools/mdprep/gromacs.util.hpp"
#include <optional>
#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gromacs
{

	struct GromacsCommandArgs
	{
		std::vector<std::string>			arguments;
		std::string							stdout_;
		std::string							stderr_;
		std::optional<InteractiveArguments> interactive_settings;
		bool								operator==( const GromacsCommandArgs & ) const noexcept = default;
	};

	// Blindly execute gromacs with input arguments.
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files as well.
	void submitGromacsCommand( const fs::path & p_gmx_exe, GromacsCommandArgs & p_args );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
