#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_PACK__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_PACK__

#include <filesystem>
#include <util/filesystem.hpp>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	struct CumulativeOuputFiles;

	struct MdInstructions
	{
		struct Equil
		{
			uint64_t nsteps	   = 50000; // with dt = 2fs => 100ns nvt duration
			double	 dt		   = 0.001; // step time interval in picoseconds
			uint64_t nstxout   = 100;
			uint64_t nstvout   = 1000;
			uint64_t nstenergy = 1000;
			uint64_t nstlog	   = 1000;
		};

		struct Prod
		{
			uint64_t nsteps	   = 50000; // with dt = 2fs => 100ns nvt duration
			double	 dt		   = 0.001; // step time interval in picoseconds
			uint64_t nstxout   = 0;
			uint64_t nstvout   = 1000;
			uint64_t nstenergy = 1000;
			uint64_t nstlog	   = 1000;
			uint64_t nstxout_compressed
				= 100; // Note for naming convention : Here I chose to name parameters after their original gromacs
					   // names. Therefore, I'd rather keeping the snake_tail here.
		};

		Equil		nvt;
		Equil		npt;
		Prod		prod;
		std::string fileStem;
	};

	struct ErrorReport
	{
		bool		error = false;
		std::string errMsg;
	};

	// Create mdp file from template at location.
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createNvtMdp( const FilePath & p_dest, MdInstructions & ) noexcept;

	// Create mdp file from template at location
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createNptMdp( const FilePath & p_dest, MdInstructions & ) noexcept;

	// Create mdp file from template at location
	// Return false if the creation failed (the template file is probably not found)
	ErrorReport createProdMdp( const FilePath & p_dest, MdInstructions & ) noexcept;

	// Create .py file responsible for running min-nvt-npt-prod from template
	ErrorReport createWorkflowPy( const FilePath & p_dest, MdInstructions & ) noexcept;

	// Pack everything needed to run MD
	// Assumes the destination directory is empty
	ErrorReport pack( const FilePath & p_dest, const CumulativeOuputFiles &, MdInstructions & ) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
