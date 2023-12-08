#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <filesystem>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep
{
	// Show gromacs where to look for forcefields
	void declare_ff_directory( const fs::path & ) noexcept;

	// Return the position of default forcefields packaged with vtx, relative to the vtx executable folder
	const fs::path & default_ff_directory_relative_path() noexcept;

	// Return the absolute path of the directory containing VTX executable
	const fs::path & executable_directory() noexcept;

} // namespace VTX::Tool::Mdprep

#endif
