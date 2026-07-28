#ifndef __VTX_IO_UTIL_BOND_RECOMPUTATION__
#define __VTX_IO_UTIL_BOND_RECOMPUTATION__

#include <core/struct/topology.hpp>
#include <util/math/grid.hpp>
#include <vector>

namespace VTX::IO::Util::BondRecomputation
{
	/**
	 * @brief Chose items to recompute bonds for.
	 */
	struct BondRecomputeFilter
	{
		std::vector<Core::ChemDB::Atom::SYMBOL>	   atomSymbols;
		std::vector<Core::ChemDB::Residue::SYMBOL> residueSymbols;
		std::vector<Core::ChemDB::Category::TYPE>  categories		 = { Core::ChemDB::Category::TYPE::UNKNOWN };
		bool									   includeDisulfides = true;
	};

	/**
	 * @brief Recompute bonds for a given topology and frame.
	 */
	void recomputeBonds(
		Core::Struct::Topology &			 p_topology,
		std::span<const Vec3f>				 p_frame,
		const VTX::Util::Math::Grid<Index> & p_atomGrid,
		const BondRecomputeFilter &			 p_filter = BondRecomputeFilter {}
	);

} // namespace VTX::IO::Util::BondRecomputation

#endif
