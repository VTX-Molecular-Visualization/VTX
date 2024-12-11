#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "types.hpp"
#include <string>

namespace VTX::Util::Filesystem
{
	// Return path to executable
	FilePath getExecutableDir();

	// Return filename without extension
	std::string getFileName( const FilePath & p_filePath );

	// Read file at path and return content as a string
	const std::string readPath( const FilePath & p_filePath );

	// Remove all files at path (if directory, remove all file in it, if file, remove all sibling files)
	void removeAll( const FilePath & p_filePath );

	// Ensure that p_filepath will not have the name of any other file in its directory (rename it if there is a file
	// with the same name)
	void generateUniqueFileName( FilePath & p_filePath );

	// Clean destination and copy directory p_filePathSrc at path p_filePathDestination recursively
	void copyDirectory( const FilePath & p_filePathSrc, const FilePath & p_filePathDestination );

	// Write a line to a file
	void writeLineToFile( const FilePath & p_filePath, const std::string & p_line );
} // namespace VTX::Util::Filesystem

#endif
