#ifndef __VTX_TOOL_TOOLS_MDPREP_JOBS__
#define __VTX_TOOL_TOOLS_MDPREP_JOBS__

#include <filesystem>
#include <functional>
#include <string>
#include <util/datalocker.hpp>
#include <util/filesystem.hpp>
#include <vector>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::backends::Gromacs
{
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
		FilePath						 lastUncompiledTop;
	};

	struct GromacsJobData
	{
		std::vector<std::string> arguments;
		std::vector<size_t> expectedOutputFilesIndexes; // Meant to point toward specific argument indexes that hold
														// path of output files to check
		VTX::Util::DataLocker<Channels>													  channelsLocker;
		JobReport																		  report;
		std::function<void( const FilePath &, GromacsJobData &, CumulativeOuputFiles & )> postJobRoutine
			= []( const FilePath &, GromacsJobData &, CumulativeOuputFiles & ) {};
		bool operator==( const GromacsJobData & ) const noexcept = default;
	};

	/**
	 * @brief Execute gromacs with input arguments then check if job issued and error
	  Assumes relevant arguments have been provided and checked beforehand.
	  Assumes gromacs have been instructed on where to find data files (env. var. GMXLIB) as well.
	  Error issued by the job can be a specific string in output channels or if expected output files doesn't exists
	  or are empty.
	  Suspends the execution until the job is finished or if the job failed to start.
	 */
	void submitGromacsJob( const FilePath & p_gmx_exe, GromacsJobData & p_args );
} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
