#ifndef __VTX_CORE_FLAT_STRUCT_MOLECULE__
#define __VTX_CORE_FLAT_STRUCT_MOLECULE__

#include "core/chemdb/atom.hpp"
#include "core/chemdb/bond.hpp"
#include "core/chemdb/category.hpp"
#include "core/chemdb/residue.hpp"
#include "core/struct/category.hpp"
#include "core/struct/trajectory.hpp"
#include <array>
#include <memory>
#include <string>
#include <vector>

namespace VTX::Core::FlatStruct
{
	struct Molecule
	{
		static const int CATEGORY_COUNT = int( ChemDB::Category::TYPE::COUNT );

		Molecule();

		std::string name = "unknown";
		Mat4f		transform;

		// Trajectory
		Struct::Trajectory trajectory = Struct::Trajectory();

		// Categories
		std::array<std::unique_ptr<Struct::Category>, CATEGORY_COUNT> categories
			= std::array<std::unique_ptr<Struct::Category>, CATEGORY_COUNT>();

		Struct::Category & getCategory( const ChemDB::Category::TYPE p_categoryType );

		// Chain data
		std::vector<std::string> chainNames			= std::vector<std::string>();
		std::vector<size_t>		 chainFirstResidues = std::vector<size_t>();
		std::vector<size_t>		 chainResidueCounts = std::vector<size_t>();

		void   initChains( const size_t p_count );
		void   appendNewChain();
		size_t getChainCount() const;

		// Residue data
		std::vector<ChemDB::Residue::SYMBOL> residueSymbols			 = std::vector<ChemDB::Residue::SYMBOL>();
		std::vector<size_t>					 residueChainIndexes	 = std::vector<size_t>();
		std::vector<size_t>					 residueFirstAtomIndexes = std::vector<size_t>();
		std::vector<size_t>					 residueAtomCounts		 = std::vector<size_t>();
		std::vector<size_t>					 residueFirstBondIndexes = std::vector<size_t>();
		std::vector<size_t>					 residueBondCounts		 = std::vector<size_t>();

		void   initResidues( const size_t p_count );
		size_t getResidueCount() const;

		// Atom data
		std::vector<ChemDB::Atom::SYMBOL> atomSymbols		 = std::vector<ChemDB::Atom::SYMBOL>();
		std::vector<size_t>				  atomResidueIndexes = std::vector<size_t>();
		std::vector<std::string>		  atomNames			 = std::vector<std::string>();

		void   initAtoms( const size_t p_count );
		size_t getAtomCount() const;

		// Bond data
		std::vector<ChemDB::Bond::ORDER> bondOrders			   = std::vector<ChemDB::Bond::ORDER>();
		std::vector<size_t>				 bondFirstAtomIndexes  = std::vector<size_t>();
		std::vector<size_t>				 bondSecondAtomIndexes = std::vector<size_t>();

		void   initBonds( const size_t p_count );
		size_t getBondCount() const;

		// TODO Manage Atom types and residue types with sparse_set
	};
} // namespace VTX::Core::FlatStruct
#endif
