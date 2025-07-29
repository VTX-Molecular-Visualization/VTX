#pragma once
#include <vtx/secondary_structure/shared/shared.hpp>

namespace pdb100
{

	/**
	 * @brief Use data from input file to fill secondary structure data
	 * @param p_systemPath
	 * @param p_helixes
	 */
	void parse( const fs::path & p_systemPath, std::vector<Helix> & p_helixes );
	/**
	 * @brief Use data from input file to fill secondary structure data
	 * @param p_systemPath
	 * @param p_helixes
	 */
	void parse( const fs::path & p_systemPath, std::vector<Strand> & p_strands );
} // namespace pdb100
