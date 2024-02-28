#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include <filesystem>
#include <optional>
#include <string>
#include <util/datalocker.hpp>
#include <vector>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::Gromacs
{
	class Inputs;

	struct Channels
	{
		std::string stdout_;
		std::string stderr_;
	};
	struct GromacsCommandArgs
	{
		std::vector<std::string>		arguments;
		VTX::Util::DataLocker<Channels> channelsLocker;
		std::optional<Inputs>
			 interactiveSettings; // If the Inputs class is instanciated, the process is expected to be interactive.
		bool operator==( const GromacsCommandArgs & ) const noexcept = default;
	};

	// Blindly execute gromacs with input arguments.
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files (env. var. GMXLIB) as well.
	void submitGromacsCommand( const fs::path & p_gmx_exe, GromacsCommandArgs & p_args );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
