#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <filesystem>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep
{
	// Return the absolute path of the directory containing VTX executable
	// Path separator is garanteed to be the one prefered by the system
	const fs::path & executableDirectory() noexcept;

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	// Return the position of default forcefields packaged with vtx, relative to the vtx executable folder
	const fs::path & defaultFfDirectoryRelativePath() noexcept;

	// Return the position of gmx binary relative to the vtx executable folder
	const fs::path & defaultGmxBinaryRelativePath() noexcept;

	// Show gromacs where to look for forcefields
	void declareFfDirectory( const fs::path & ) noexcept;

	// Fill the job report with results on the job data. Aims to indentify if all expected output files are here and if
	// there is no error message in any of the chanels.
	void checkJobResults( GromacsJobData & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
