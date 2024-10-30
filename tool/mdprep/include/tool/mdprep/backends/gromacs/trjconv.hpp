#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_TRJCONV__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_TRJCONV__

#include <filesystem>
namespace fs = std::filesystem;

class QProcess;

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	// Support solvation with the default spc216.gro water system
	struct TrjconvInstructions
	{
		std::string fileStem;
		fs::path	inputGro;
		fs::path	outputGro;
	};

	// Setup job folder and place input files as the job need them
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, TrjconvInstructions & ) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	void convert( const TrjconvInstructions &, GromacsJobData & ) noexcept;

	struct TrjconvInputs
	{
		bool centeringDone = false;
	};
	bool isWaitingForInput( const TrjconvInputs &, const std::string_view & p_stdout ) noexcept;
	bool enterInput(
		TrjconvInputs & p_inputs,
		QProcess &		p_proc,
		std::string &	p_stdout,
		std::string &	p_stderr
	) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
