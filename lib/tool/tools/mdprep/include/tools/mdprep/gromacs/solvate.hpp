#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_SOLVATE__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_SOLVATE__

#include <filesystem>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	// Support solvation with the default spc216.gro water molecule
	struct solvateInstructions
	{
		fs::path inputGro;
		fs::path inputTop;
		fs::path outputGro;
	};

	// Setup job folder and place input files as the job need them
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, solvateInstructions & ) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	//    If the output_gro is empty, will use the input filename root and append "solv"
	void convert( const solvateInstructions &, GromacsJobData & ) noexcept;
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
