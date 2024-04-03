#ifndef __VTX_TOOL_TOOLS_MDPREP_JOBS__
#define __VTX_TOOL_TOOLS_MDPREP_JOBS__

#include <filesystem>
#include <functional>
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
		bool					 finished	  = false;
		bool					 errorOccured = false;
		std::vector<std::string> errors;
	};

	// The idea is to list output files from all previous job, in reverse chronological order. So we can access last
	// files first. This is usefull because some job might need some files from a couple of jobs ago, but the last file
	// of a given extension is always the right file. Therefore we can identify the right file to use for the job by
	// taking the first one that matches the required extension.
	struct CumulativeOuputFiles
	{
		std::vector<const std::string *> fileStringPtrs;
		fs::path						 lastUncompiledTop;
	};

	struct GromacsJobData
	{
		std::vector<std::string> arguments;
		std::vector<size_t> expectedOutputFilesIndexes; // Meant to point toward specific argument indexes that hold
														// path of output files to check
		VTX::Util::DataLocker<Channels> channelsLocker;
		JobReport						report;
		std::optional<Inputs>
			interactiveSettings; // If the Inputs class is instanciated, the process is expected to be interactive.
		std::function<void( const fs::path &, GromacsJobData &, CumulativeOuputFiles & )> postJobRoutine
			= []( const fs::path &, GromacsJobData &, CumulativeOuputFiles & ) {};
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
