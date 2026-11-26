#pragma once

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
