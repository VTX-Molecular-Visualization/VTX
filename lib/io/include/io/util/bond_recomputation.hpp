#ifndef __VTX_IO_UTIL_BOND_RECOMPUTATION__
#define __VTX_IO_UTIL_BOND_RECOMPUTATION__

#include <core/struct/topology.hpp>
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
		Core::Struct::Topology &	p_topology,
		const Core::Struct::Frame & p_frame,
		const BondRecomputeFilter & p_filter = BondRecomputeFilter {}
	);

} // namespace VTX::IO::Util::BondRecomputation

#endif
