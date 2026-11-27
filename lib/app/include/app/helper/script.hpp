#pragma once

#include <util/filesystem.hpp>

namespace VTX::App::Helper::Script
{
	/**
	 * @brief Check if the script is a built-in.
	 */
	bool isReadOnly( const FilePath & p_path );

	/**
	 * @brief Copy a script into the user scripts directory and return the copy path.
	 */
	FilePath copyToUserDirectory( const FilePath & p_path );

	/**
	 * @brief Duplicate a script next to it, or into the user scripts directory if it is read-only.
	 */
	FilePath duplicate( const FilePath & p_path );
} // namespace VTX::App::Helper::Script
