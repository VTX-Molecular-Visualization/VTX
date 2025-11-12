#ifndef __VTX_CORE_STRUCT_SYSTEM__
#define __VTX_CORE_STRUCT_SYSTEM__

#include "core/chemdb/atom.hpp"
#include "core/chemdb/bond.hpp"
#include "core/chemdb/category.hpp"
#include "core/chemdb/residue.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include "core/struct/trajectory.hpp"
#include <array>
#include <memory>
#include <string>
#include <util/math/range_list.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Raw data structure representing a molecular system.
	 */
	struct System
	{
		// TODO: merge here.
		Struct::Trajectory trajectory;

		/**
		 * @brief System name.
		 */
		std::string name;

		/**
		 * @brief Chains indexes by category.
		 */
		std::array<std::vector<Index>, uint( ChemDB::Category::TYPE::COUNT )> categories;
		/**
		 * @brief Chain names.
		 */
		std::vector<std::string> chainNames;
		/**
		 * @brief Indexes of the first of the residues that compose each chain.
		 * All residues with index between vector[N] and vector[N+1] are considered part of the chain N.
		 */
		std::vector<Index> chainFirstResidues;
		/**
		 * @brief Number of residues in each chain.
		 */
		std::vector<Index> chainResidueCounts;
		/**
		 * @brief Residue symbols.
		 */
		std::vector<ChemDB::Residue::SYMBOL> residueSymbols;
		/**
		 * @brief Residue names.
		 */
		std::vector<std::string> residueNames;
		/**
		 * @brief Index of the chain each residue belongs to.
		 */
		std::vector<Index> residueChainIndexes;
		/**
		 * @brief Original residue IDs from the input file.
		 */
		std::vector<Index> residueOriginalIds;
		/**
		 * @brief Indexes of the first of the atoms that compose each residues.
		 * All atoms with index between vector[N] and vector[N+1] are considered part of the residue N.
		 */
		std::vector<Index> residueFirstAtomIndexes;
		/**
		 * @brief Number of atoms in each residue.
		 */
		std::vector<Index> residueAtomCounts;
		/**
		 * @brief Indexes of the first of the bond that compose each residues. All bonds with index between vector[N]
		 * and vector[N+1] are considered part of the residue N
		 */
		std::vector<Index> residueFirstBondIndexes;
		/**
		 * @brief Number of bonds in each residue.
		 */
		std::vector<Index> residueBondCounts;
		/**
		 * @brief Secondary structure types for residues.
		 */
		std::vector<ChemDB::SecondaryStructure::TYPE> residueSecondaryStructureTypes;
		/**
		 * @brief All atom symbols.
		 */
		std::vector<ChemDB::Atom::SYMBOL> atomSymbols;
		/**
		 * @brief A vector containing the residue indexes for each atom.
		 */
		std::vector<Index> atomResidueIndexes;
		/**
		 * @brief Atom names.
		 */
		std::vector<std::string> atomNames;
		/**
		 * @brief All bond orders.
		 */
		std::vector<ChemDB::Bond::ORDER> bondOrders;
		/**
		 * @brief Each bond is represented by two consecutive atom indexes.
		 */
		std::vector<Index> bondPairAtomIndexes;

		// TODO Manage Atom types and residue types with sparse_set
		Util::Math::RangeList<Index> atomSolvents;
		Util::Math::RangeList<Index> atomIons;

		/**
		 * @brief Convenient methods to reserve data vectors.
		 */
		void appendNewChain();
		void initChains( const Index p_count );
		void initResidues( const Index p_count );
		void initAtoms( const Index p_count );
		void initBonds( const Index p_count );

		/**
		 * @brief All needed getters.
		 */
		inline Index					  getChainCount() const { return Index( chainNames.size() ); }
		inline Index					  getResidueCount() const { return Index( residueSymbols.size() ); }
		inline Index					  getAtomCount() const { return Index( atomSymbols.size() ); }
		inline Index					  getBondCount() const { return Index( bondOrders.size() ); }
		inline const std::vector<Index> & getChainIndexesFromCategory( const ChemDB::Category::TYPE p_category ) const
		{
			return categories[ uint( p_category ) ];
		}
	};

	ByteNumber dynamicMemoryUsage( const System & ) noexcept;

} // namespace VTX::Core::Struct
#endif
