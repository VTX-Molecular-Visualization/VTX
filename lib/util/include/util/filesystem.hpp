#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "types.hpp"
#include <string>

namespace VTX::Util::Filesystem
{
	/**
	 * @brief Return path to the executable folder.
	 */
	FilePath getExecutableDir();

	/**
	 * @brief Return path to the executable file.
	 */
	FilePath getExecutable();

	/**
	 * @brief Get local directories.
	 */
	FilePath getConfigHome();
	FilePath getDataHome();
	FilePath getStateDir();
	FilePath getCacheDir();
	FilePath getDocumentsFolder();
	FilePath getDesktopFolder();
	FilePath getPicturesFolder();
	FilePath getMusicFolder();
	FilePath getVideoFolder();
	FilePath getDownloadFolder();
	FilePath getSaveGamesFolder1();
	FilePath getSaveGamesFolder2();

	/**
	 * @brief Read content of file at path.
	 */
	const std::string readPath( const FilePath & p_filePath );

	/**
	 * @brief Write content to file at path (override if exist).
	 */
	void writeFile( const FilePath & p_filePath, const std::string & p_content );

	/**
	 * @brief If file at path already exists, generate a new name.
	 */
	void generateUniqueFileName( FilePath & p_filePath );

	/**
	 * @brief Copy directory from source to destination (override if exist).
	 */
	void copyDirectory( const FilePath & p_filePathSrc, const FilePath & p_filePathDestination );

} // namespace VTX::Util::Filesystem

#endif
