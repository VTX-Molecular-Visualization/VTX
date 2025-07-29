#pragma once
#include <vtx/secondary_structure/shared.hpp>

namespace pdb100
{

	/**
	 * @brief Open each directory from the dbDir and list the files in the pdb100_system collection
	 * @param contextData
	 */
	void enumerateFiles( Context & contextData );
} // namespace pdb100
