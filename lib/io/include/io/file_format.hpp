#ifndef VTX_IO_FILE_FORMAT_HPP
#define VTX_IO_FILE_FORMAT_HPP

#include <util/filesystem.hpp>
#include <util/types.hpp>

namespace VTX::IO
{
	/**
	 * @brief Check if the file format is a script file format.
	 */
	bool isScriptFileFormat( const FilePath & p_path );

	/**
	 * @brief Check if the file format is a trajectory file format.
	 */
	bool isTrajectoryFileFormat( const FilePath & p_path );
} // namespace VTX::IO

#endif
