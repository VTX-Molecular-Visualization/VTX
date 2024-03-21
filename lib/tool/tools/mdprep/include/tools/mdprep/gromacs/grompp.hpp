#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMPP__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMPP__

#include <filesystem>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	struct GromppInstructionsMinimization
	{
		double	 emtol	= 1000.0; // Stop minimization when the maximum force < 1000.0 kJ/mol/nm
		double	 emstep = 0.01;	  // Minimization step size
		uint64_t nsteps = 50000;  // Maximum number of (minimization) steps to perform
	};

	struct GromppInstructions
	{
		std::string fileStem;
		fs::path	inputGro;
		fs::path	inputMdp;
		fs::path	inputTop;
		fs::path	outputTpr;
		fs::path	outputMdp;
		fs::path	outputTop;

		GromppInstructionsMinimization min;
	};

	// Setup job folder and place input files as the job need them
	// Generate the .mdp file from template based on the user inputs in GromppInstructionsMinimization
	// if .mdp found in previous output files, will use it instead
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, GromppInstructions & ) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	void convert( const GromppInstructions &, GromacsJobData & ) noexcept;

} // namespace VTX::Tool::Mdprep::Gromacs

#endif
