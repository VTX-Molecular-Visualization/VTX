#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMPP__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GROMPP__

#include <filesystem>
#include <util/filesystem.hpp>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	struct GromppInstructionsMinimization
	{
		double	 emtol	= 1000.0; // Stop minimization when the maximum force < 1000.0 kJ/mol/nm
		double	 emstep = 0.01;	  // Minimization step size
		uint64_t nsteps = 50000;  // Maximum number of (minimization) steps to perform
	};

	enum class E_GROMPP_STEP
	{
		ions,
		posres, // We need to compile a .top file for topology with posres, and for topology without constraints.
		em,		// .top file from this step will be used for unconstrained MDs. All other files will be used in mdrun em
	};

	struct GromppInstructions
	{
		std::string fileStem;
		FilePath	inputGro;
		FilePath	inputMdp;
		FilePath	inputTop;
		FilePath	outputTpr;
		FilePath	outputMdp;
		FilePath	outputTop;

		E_GROMPP_STEP				   step = E_GROMPP_STEP::ions;
		GromppInstructionsMinimization min;
	};

	// Setup job folder and place input files as the job need them
	// Generate the .mdp file from template based on the user inputs in GromppInstructionsMinimization
	// if .mdp found in previous output files, will use it instead
	void prepareJob(
		const CumulativeOuputFiles &,
		const FilePath &		 p_root,
		const std::string_view & p_folderName,
		GromppInstructions &
	) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	void convert( const GromppInstructions &, GromacsJobData & ) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
