#pragma once
#include <vtx/secondary_structure/shared.hpp>

namespace pdb100
{

	/**
	 * @brief Use libarchive to decompress the directory , take the first file and write it at destination.
	 * @param src
	 * @param dest
	 */
	void decompressFile( const fs::path & src, const fs::path & dest );

	/**
	 * @brief Open each directory from the dbDir and list the files in the pdb100_system collection
	 * @param contextData
	 */
	void enumerateFiles( Context & contextData );
} // namespace pdb100
