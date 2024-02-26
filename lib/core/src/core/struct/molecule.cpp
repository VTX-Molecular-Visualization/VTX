#include "core/struct/molecule.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
	Molecule::Molecule()
	{
		for ( size_t i = 0; i < CATEGORY_COUNT; i++ )
		{
			categories[ i ] = std::make_unique<Struct::Category>( ChemDB::Category::TYPE( i ) );
		}
	}
	// Categories
	Struct::Category & Molecule::getCategory( const ChemDB::Category::TYPE p_categoryType )
	{
		return *( categories[ size_t( p_categoryType ) ] );
	}

	// Chain data
	void Molecule::initChains( const size_t p_count )
	{
		chainNames.resize( p_count );
		chainFirstResidues.resize( p_count, INVALID_INDEX );
		chainResidueCounts.resize( p_count, 0 );
	}
	void Molecule::appendNewChain()
	{
		chainNames.emplace_back( "" );
		chainFirstResidues.emplace_back( INVALID_INDEX );
		chainResidueCounts.emplace_back( 0 );
	}
	size_t Molecule::getChainCount() const { return chainNames.size(); }

	// Residue data
	void Molecule::initResidues( const size_t p_count )
	{
		residueSymbols.resize( p_count, ChemDB::Residue::SYMBOL::UNKNOWN );
		residueChainIndexes.resize( p_count, INVALID_INDEX );
		residueFirstAtomIndexes.resize( p_count, INVALID_ATOM_INDEX );
		residueAtomCounts.resize( p_count, 0 );
		residueFirstBondIndexes.resize( p_count, INVALID_INDEX );
		residueBondCounts.resize( p_count, 0 );
		residueOriginalIds.resize( p_count, 0 );
		residueSecondaryStructureTypes.resize( p_count, ChemDB::SecondaryStructure::TYPE::UNKNOWN );
	}
	size_t Molecule::getResidueCount() const { return residueSymbols.size(); }

	// Atom data
	void Molecule::initAtoms( const size_t p_count )
	{
		assert( p_count < std::numeric_limits<atom_index_t>::max() );

		atomSymbols.resize( p_count, ChemDB::Atom::SYMBOL::UNKNOWN );
		atomResidueIndexes.resize( p_count, INVALID_INDEX );
		atomNames.resize( p_count );
	}
	size_t Molecule::getAtomCount() const { return atomSymbols.size(); }

	// Bond data
	void Molecule::initBonds( const size_t p_count )
	{
		bondOrders.resize( p_count, ChemDB::Bond::ORDER::UNKNOWN );
		bondPairAtomIndexes.resize( p_count * 2, INVALID_ATOM_INDEX );
	}
	size_t Molecule::getBondCount() const { return bondOrders.size(); }

} // namespace VTX::Core::Struct