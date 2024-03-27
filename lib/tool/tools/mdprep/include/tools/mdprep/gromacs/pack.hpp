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
		uint64_t	 nvtNsteps	= 50000;	 // with dt = 2fs => 100ns nvt duration
		uint64_t	 nptNsteps	= 50000;	 // with dt = 2fs => 100ns nvt duration
		uint64_t	 prodNsteps = 250000000; // with dt = 2fs => 500ns nvt duration
		const double dtNs		= 0.002;
		std::string	 fileStem;
	};
	struct ErrorReport
	{
		bool		error = false;
		std::string errMsg;
	};

	// Create mdp file from template at location.
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createNvtMdp( const fs::path & p_dest, MdInstructions & ) noexcept;

	// Create mdp file from template at location
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createNptMdp( const fs::path & p_dest, MdInstructions & ) noexcept;

	// Create mdp file from template at location
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createProdMdp( const fs::path & p_dest, MdInstructions & ) noexcept;

	// Create .py file responsible for running min-nvt-npt-prod from template
	ErrorReport createWorkflowPy( const fs::path & p_dest, MdInstructions & ) noexcept;

	// Pack everything needed to run MD
	// Assumes the destination directory is empty as it will overwrite files inside if name conflict occurs
	ErrorReport pack( const fs::path & p_dest, const CumulativeOuputFiles &, MdInstructions & ) noexcept;
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
