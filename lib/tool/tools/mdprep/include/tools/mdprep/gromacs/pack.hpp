#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_PACK__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_PACK__

#include <filesystem>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::Gromacs
{

	struct CumulativeOuputFiles;
	struct MdInstructions
	{
		// For now the only customizable parameter is the number of steps. And it probably won't be show to the user as
		// is. Maybe asking him for the duration is more friendly.
		uint64_t nvtNsteps	= 50000;	 // with dt = 2fs => 100ns nvt duration
		uint64_t nptNsteps	= 50000;	 // with dt = 2fs => 100ns nvt duration
		uint64_t prodNsteps = 250000000; // with dt = 2fs => 500ns nvt duration
	};

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
