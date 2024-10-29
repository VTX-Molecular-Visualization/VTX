#ifndef __VTX_CORE_STRUCT_MOLECULE__
#define __VTX_CORE_STRUCT_MOLECULE__

#include "core/chemdb/atom.hpp"
#include "core/chemdb/bond.hpp"
#include "core/chemdb/category.hpp"
#include "core/chemdb/residue.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include "core/struct/category.hpp"
#include "core/struct/trajectory.hpp"
#include <array>
#include <memory>
#include <string>
#include <util/math/range_list.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	struct System
	{
		static const int CATEGORY_COUNT = int( ChemDB::Category::TYPE::COUNT );

		System();

		std::string name	  = "unknown";
		Mat4f		transform = MAT4F_ID;

		// Trajectory
		Struct::Trajectory trajectory;

		// Categories
		std::array<std::unique_ptr<Struct::Category>, CATEGORY_COUNT> categories;

		Struct::Category &		 getCategory( const ChemDB::Category::TYPE p_categoryType );
		const Struct::Category & getCategory( const ChemDB::Category::TYPE p_categoryType ) const;

		// Chain data - all indexes are in sync
		std::vector<std::string> chainNames;
		/**
		 * @brief Indexes of the first of the residues that compose each chain. All residues with index between
		 * vector[N] and vector[N+1] are considered part of the chain N
		 */
		std::vector<size_t> chainFirstResidues;
		std::vector<size_t> chainResidueCounts;

		void   initChains( const size_t p_count );
		void   appendNewChain();
		size_t getChainCount() const;

		// Residue data - all indexes are in sync
		std::vector<ChemDB::Residue::SYMBOL> residueSymbols;
		std::vector<size_t>					 residueChainIndexes;
		/**
		 * @brief Indexes of the first of the atoms that compose each residues. All atoms with index between vector[N]
		 * and vector[N+1] are considered part of the residue N
		 */
		std::vector<atom_index_t> residueFirstAtomIndexes;
		std::vector<atom_index_t> residueAtomCounts;
		/**
		 * @brief Indexes of the first of the bond that compose each residues. All bonds with index between vector[N]
		 * and vector[N+1] are considered part of the residue N
		 */
		std::vector<size_t>							  residueFirstBondIndexes;
		std::vector<size_t>							  residueBondCounts;
		std::vector<size_t>							  residueOriginalIds;
		std::vector<ChemDB::SecondaryStructure::TYPE> residueSecondaryStructureTypes;
		std::vector<std::string>					  residueUnknownNames; // TODO : migrate toward unordered_map

		void   initResidues( const size_t p_count );
		size_t getResidueCount() const;

		// Atom data - all indexes are in sync
		std::vector<ChemDB::Atom::SYMBOL> atomSymbols;
		std::vector<size_t>				  atomResidueIndexes;
		std::vector<std::string>		  atomNames;

		void   initAtoms( const size_t p_count );
		size_t getAtomCount() const;

		// Bond data
		std::vector<ChemDB::Bond::ORDER> bondOrders;
		/**
		 * @brief This vector works by associating atomidx at vector[N] with vector[N+1] where N is even.
		 */
		std::vector<atom_index_t> bondPairAtomIndexes;

		void   initBonds( const size_t p_count );
		size_t getBondCount() const;

		// TODO Manage Atom types and residue types with sparse_set
		Util::Math::RangeList<atom_index_t> atomSolvents;
		Util::Math::RangeList<atom_index_t> atomIons;
	};
} // namespace VTX::Core::Struct
#endif
