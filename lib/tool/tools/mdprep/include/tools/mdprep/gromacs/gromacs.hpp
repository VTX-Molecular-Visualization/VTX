#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS__

#include <filesystem>
#include <optional>
#include <string>
#include <util/datalocker.hpp>
#include <vector>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::Gromacs
{
	class Inputs;

	struct Channels
	{
		std::string stdout_;
		std::string stderr_;
	};
	struct JobReport
	{
		bool					 finished	   = false;
		bool					 error_occured = false;
		std::vector<std::string> errors;
		bool					 allOutputOk
			= false; // Ouput file(s) are considered Ok if they exists in the file system and are non-null in size
	};
	struct GromacsJobData
	{
		std::vector<std::string> arguments;
		std::vector<std::string *>
			expectedOutputFilesPtrs; // Meant to point toward specific argument that hold path of output files to check
		VTX::Util::DataLocker<Channels> channelsLocker;
		JobReport						report;
		std::optional<Inputs>
			 interactiveSettings; // If the Inputs class is instanciated, the process is expected to be interactive.
		bool operator==( const GromacsJobData & ) const noexcept = default;
	};

	// Execute gromacs with input arguments then check if job issued and error
	//   Assumes relevant arguments have been provided and checked beforehand.
	//   Assumes gromacs have been instructed on where to find data files (env. var. GMXLIB) as well.
	//   Error issued by the job can be a specific string in output channels or if expected output files doesn't exists
	//   or are empty.
	void submitGromacsJob( const fs::path & p_gmx_exe, GromacsJobData & p_args );
} // namespace VTX::Tool::Mdprep::Gromacs

#endif
