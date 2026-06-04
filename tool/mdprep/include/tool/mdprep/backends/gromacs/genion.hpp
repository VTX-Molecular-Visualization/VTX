#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_GENION__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_GENION__

#include <filesystem>
#include <optional>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	struct GromacsJobData;
	struct CumulativeOuputFiles;
	struct GenionInstructions
	{
		std::string			  fileStem;
		fs::path			  inputTpr;
		fs::path			  inputTop;
		fs::path			  outputGro;
		std::string			  pname { "NA" };
		int8_t				  pq = 1;
		std::string			  nname { "CL" };
		int8_t				  nq = -1;
		std::optional<double> conc; // If not here, it means that the user only want to neutralize the system
	};

	// Setup job folder and place input files as the job need them
	void
	prepareJob( const CumulativeOuputFiles &, const fs::path & p_root, const std::string_view & p_folderName, GenionInstructions & ) noexcept;

	// Write gromacs command arguments using input instructions
	//    Does nothing if the instructions have default values.
	//    Does not perform filesystem check on input gro
	void convert( const GenionInstructions &, GromacsJobData & ) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs
#endif
