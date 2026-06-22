#ifndef __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__
#define __VTX_TOOL_TOOLS_MDPREP_GROMACS_UTIL__

#include <filesystem>
#include <util/filesystem.hpp>
namespace fs = std::filesystem;

namespace VTX::Tool::Mdprep::backends::Gromacs
{
	struct GromacsJobData;
	struct CumulativeOuputFiles;

	FilePath defaultFfDirectoryPath();
	FilePath defaultGmxBinaryPath();
	FilePath defaultGmxTemplatesPath();

	// Show gromacs where to look for forcefields
	void declareFfDirectory( const FilePath & ) noexcept;

	// Fill the job report with results on the job data. Aims to indentify if all expected output files are here and if
	// there is no error message in any of the chanels.
	void checkJobResults( GromacsJobData & ) noexcept;

	// TODO Integrate this function on other prepareJob
	// Return the first string with matching ending from the list.
	const std::string * getFirstFileOfType( const CumulativeOuputFiles &, const char * suffix ) noexcept;

	// Return the n-th string with matching ending from the list.
	const std::string * getFileOfType( const CumulativeOuputFiles &, const size_t & n, const char * suffix ) noexcept;

	// Return a string containing the text file's content.
	// Doesn't check if the file isn't too large
	std::string getFileContent( const FilePath & ) noexcept;

	// Write string content into file.
	// Erase file's currrent content if file already exists
	void writeIntoFile( const FilePath &, const std::string & ) noexcept;

	void setLastArgumentAsExpectedOutputFile( GromacsJobData & ) noexcept;

	// return absolute path to a temporary directory that is guaranteed to be new and empty
	FilePath createNewEmptyTempDirectory() noexcept;

	// replace null-terminated characters by the repl argument into the string
	void replace( std::string & p_text, const char * p_pattern, const std::string & p_repl ) noexcept;

} // namespace VTX::Tool::Mdprep::backends::Gromacs

#endif
