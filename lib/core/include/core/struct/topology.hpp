#ifndef __VTX_CORE_STRUCT_TOPOLOGY__
#define __VTX_CORE_STRUCT_TOPOLOGY__

#include "core/chemdb/atom.hpp"
#include "core/chemdb/bond.hpp"
#include "core/chemdb/category.hpp"
#include "core/chemdb/residue.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <util/math/range_list.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Items.
	 */
	enum struct E_SYSTEM_ITEM : uint8_t
	{
		SYSTEM,
		CATEGORY,
		CHAIN,
		RESIDUE,
		ATOM,
		BOND

	};
	/**
	 * @brief Element index ranges.
	 */
	using IndexRange	 = Util::Math::Range<Index>;
	using IndexRangeList = Util::Math::RangeList<Index>;

	/**
	 * @brief Raw data structure representing a molecular system.
	 */
	struct Topology
	{
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
		 * @brief Residue indexes by category.
		 */
		std::array<std::vector<Index>, toUnderlying( ChemDB::Category::TYPE::COUNT )> categoryResidues;
		/**
		 * @brief Category of each residue.
		 */
		std::vector<ChemDB::Category::TYPE> residueCategories;
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
		 * @brief Mapping from current atom index to original source atom index after retopology.
		 */
		std::optional<std::vector<Index>> atomOriginalIndexes;
		/**
		 * @brief Two consecutive atom indexes in this vector represent a bond.
		 */
		std::vector<Index> bondPairAtomIndexes;
		/**
		 * @brief Bond orders for each bond index.
		 */
		std::vector<ChemDB::Bond::ORDER> bondOrders;

		/**
		 * @brief Convenient methods to reserve data vectors.
		 */
		void appendNewChain();
		void initChains( Index p_count );
		void initResidues( Index p_count );
		void initAtoms( Index p_count );
		void initBonds( Index p_count );

		/**
		 * @brief All needed getters.
		 */
		Index getChainCount() const { return Index( chainNames.size() ); }

		Index getResidueCount() const { return Index( residueSymbols.size() ); }

		Index getAtomCount() const { return Index( atomSymbols.size() ); }

		Index getBondCount() const { return Index( bondOrders.size() ); }

		IndexRange getAtomRange() const { return IndexRange::fromFirstCount( 0, getAtomCount() ); }

		const std::string & getChainName( const Index p_chainIndex ) const { return chainNames[ p_chainIndex ]; }

		IndexRange getChainRange() const { return IndexRange::fromFirstCount( 0, getChainCount() ); }

		Index getChainFirstResidue( const Index p_chainIndex ) const { return chainFirstResidues[ p_chainIndex ]; }

		Index getChainResidueCount( const Index p_chainIndex ) const { return chainResidueCounts[ p_chainIndex ]; }

		IndexRange getChainResidueRange( const Index p_chainIndex ) const
		{
			const Index first = getChainFirstResidue( p_chainIndex );
			const Index count = getChainResidueCount( p_chainIndex );
			return IndexRange::fromFirstCount( first, count );
		}

		Index getChainFirstAtom( const Index p_chainIndex ) const
		{
			const Index firstResidue = getChainFirstResidue( p_chainIndex );
			return residueFirstAtomIndexes[ firstResidue ];
		}

		Index getChainAtomCount( const Index p_chainIndex ) const
		{
			const Index firstResidue = getChainFirstResidue( p_chainIndex );
			const Index residueCount = getChainResidueCount( p_chainIndex );
			Index		atomCount	 = 0;

			for ( Index r = firstResidue; r < firstResidue + residueCount; ++r )
			{
				atomCount += residueAtomCounts[ r ];
			}

			return atomCount;
		}

		IndexRange getChainAtomRange( const Index p_chainIndex ) const
		{
			const Index firstAtom = getChainFirstAtom( p_chainIndex );
			const Index count	  = getChainAtomCount( p_chainIndex );
			return IndexRange::fromFirstCount( firstAtom, count );
		}

		ChemDB::Residue::SYMBOL getResidueSymbol( const Index p_residueIndex ) const
		{ return residueSymbols[ p_residueIndex ]; }

		const std::string & getResidueName( const Index p_residueIndex ) const
		{ return residueNames[ p_residueIndex ]; }

		Index getResidueChainIndex( const Index p_residueIndex ) const { return residueChainIndexes[ p_residueIndex ]; }

		Index getResidueOriginalId( const Index p_residueIndex ) const { return residueOriginalIds[ p_residueIndex ]; }

		Index getResidueFirstAtom( const Index p_residueIndex ) const
		{ return residueFirstAtomIndexes[ p_residueIndex ]; }

		Index getResidueAtomCount( const Index p_residueIndex ) const { return residueAtomCounts[ p_residueIndex ]; }

		IndexRange getResidueAtomRange( const Index p_residueIndex ) const
		{
			const Index first = getResidueFirstAtom( p_residueIndex );
			const Index count = getResidueAtomCount( p_residueIndex );
			return IndexRange::fromFirstCount( first, count );
		}

		const std::vector<Index> & getCategoryResidues( const ChemDB::Category::TYPE p_category ) const
		{ return categoryResidues[ toUnderlying( p_category ) ]; }

		ChemDB::Category::TYPE getResidueCategory( const Index p_residueIndex ) const
		{ return residueCategories[ p_residueIndex ]; }

		IndexRangeList getCategoryAtomRangeList( const ChemDB::Category::TYPE p_category ) const
		{
			IndexRangeList atoms;

			for ( const Index residue : getCategoryResidues( p_category ) )
			{
				atoms.addRange( getResidueAtomRange( residue ) );
			}
			return atoms;
		}

		IndexRangeList getCategoryAtomRangeList( const Index p_category ) const
		{ return getCategoryAtomRangeList( static_cast<ChemDB::Category::TYPE>( p_category ) ); }

		ChemDB::SecondaryStructure::TYPE getResidueSecondaryStructureType( const Index p_residueIndex ) const
		{ return residueSecondaryStructureTypes[ p_residueIndex ]; }

		ChemDB::Atom::SYMBOL getAtomSymbol( const Index p_atomIndex ) const { return atomSymbols[ p_atomIndex ]; }

		const std::string & getAtomName( const Index p_atomIndex ) const { return atomNames[ p_atomIndex ]; }

		std::optional<Index> getAtomOriginalIndex( const Index p_atomIndex ) const
		{
			if ( not atomOriginalIndexes )
			{
				return std::nullopt;
			}
			return ( *atomOriginalIndexes )[ p_atomIndex ];
		}

		Index getAtomResidueIndex( const Index p_atomIndex ) const { return atomResidueIndexes[ p_atomIndex ]; }

		Index getAtomChainIndex( const Index p_atomIndex ) const
		{
			const Index residueIndex = getAtomResidueIndex( p_atomIndex );
			return getResidueChainIndex( residueIndex );
		}

		ChemDB::Bond::ORDER getBondOrder( const Index p_bondIndex ) const { return bondOrders[ p_bondIndex ]; }

		Index getBondFirstAtom( const Index p_bondIndex ) const
		{ return bondPairAtomIndexes[ static_cast<size_t>( p_bondIndex ) * 2 ]; }

		Index getBondSecondAtom( const Index p_bondIndex ) const
		{ return bondPairAtomIndexes[ static_cast<size_t>( p_bondIndex ) * 2 + 1 ]; }

		std::optional<Index> findFirstAtomByName( const Index p_residueIndex, const std::string_view p_name ) const
		{
			for ( Index i : getResidueAtomRange( p_residueIndex ) )
			{
				if ( getAtomName( i ) == p_name )
				{
					return i;
				}
			}
			return std::nullopt;
		}
	};

	size_t dynamicMemoryUsage( const Topology & ) noexcept;

} // namespace VTX::Core::Struct
#endif
