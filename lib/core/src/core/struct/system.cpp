#include "core/struct/system.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
	System::System()
	{
		for ( size_t i = 0; i < CATEGORY_COUNT; i++ )
		{
			categories[ i ] = std::make_unique<Struct::Category>( ChemDB::Category::TYPE( i ) );
		}
	}
	// Categories
	Struct::Category & System::getCategory( const ChemDB::Category::TYPE p_categoryType )
	{
		return *( categories[ Index( p_categoryType ) ] );
	}
	const Struct::Category & System::getCategory( const ChemDB::Category::TYPE p_categoryType ) const
	{
		return *( categories[ Index( p_categoryType ) ] );
	}

	// Chain data
	void System::initChains( const Index p_count )
	{
		chainNames.resize( p_count );
		chainFirstResidues.resize( p_count, INVALID_INDEX );
		chainResidueCounts.resize( p_count, 0 );
	}
	void System::appendNewChain()
	{
		chainNames.emplace_back( "" );
		chainFirstResidues.emplace_back( INVALID_INDEX );
		chainResidueCounts.emplace_back( 0 );
	}
	Index System::getChainCount() const { return Index( chainNames.size() ); }

	// Residue data
	void System::initResidues( const Index p_count )
	{
		residueSymbols.resize( p_count, ChemDB::Residue::SYMBOL::UNKNOWN );
		residueChainIndexes.resize( p_count, INVALID_INDEX );
		residueFirstAtomIndexes.resize( p_count, INVALID_INDEX );
		residueAtomCounts.resize( p_count, 0 );
		residueFirstBondIndexes.resize( p_count, INVALID_INDEX );
		residueBondCounts.resize( p_count, 0 );
		residueOriginalIds.resize( p_count, 0 );
		residueSecondaryStructureTypes.resize( p_count, ChemDB::SecondaryStructure::TYPE::UNKNOWN );
		residueUnknownNames.resize( p_count );
	}
	Index System::getResidueCount() const { return Index( residueSymbols.size() ); }

	// Atom data
	void System::initAtoms( const Index p_count )
	{
		assert( p_count < std::numeric_limits<Index>::max() );

		atomSymbols.resize( p_count, ChemDB::Atom::SYMBOL::UNKNOWN );
		atomResidueIndexes.resize( p_count, INVALID_INDEX );
		atomNames.resize( p_count );
	}
	Index System::getAtomCount() const { return Index( atomSymbols.size() ); }

	// Bond data
	void System::initBonds( const Index p_count )
	{
		bondOrders.resize( p_count, ChemDB::Bond::ORDER::UNKNOWN );
		bondPairAtomIndexes.resize( p_count * 2, INVALID_INDEX );
	}
	Index System::getBondCount() const { return Index( bondOrders.size() ); }

	ByteNumber dynamicMemoryUsage( const System & p_sys ) noexcept
	{
		ByteNumber out = 0;
		out += p_sys.name.capacity();
		// Mat4f should have dynamic allocation, right ?
		out += dynamicMemoryUsage( p_sys.trajectory );
		for ( auto & it_str : p_sys.chainNames )
			out += it_str.capacity();
		out += p_sys.chainNames.size() * sizeof( std::string );
		out += sizeof( size_t ) * p_sys.chainFirstResidues.size();
		out += sizeof( size_t ) * p_sys.chainResidueCounts.size();
		out += sizeof( ChemDB::Residue::SYMBOL ) * p_sys.residueSymbols.size();
		out += sizeof( size_t ) * p_sys.residueChainIndexes.size();
		out += sizeof( Index ) * p_sys.residueFirstAtomIndexes.size();
		out += sizeof( Index ) * p_sys.residueAtomCounts.size();
		out += sizeof( size_t ) * p_sys.residueFirstBondIndexes.size();
		out += sizeof( size_t ) * p_sys.residueBondCounts.size();
		out += sizeof( size_t ) * p_sys.residueOriginalIds.size();
		out += sizeof( ChemDB::SecondaryStructure::TYPE ) * p_sys.residueSecondaryStructureTypes.size();
		out += sizeof( std::string ) * p_sys.residueUnknownNames.size();
		out += sizeof( ChemDB::Atom::SYMBOL ) * p_sys.atomSymbols.size();
		out += sizeof( size_t ) * p_sys.atomResidueIndexes.size();
		out += sizeof( std::string ) * p_sys.atomNames.size();
		out += sizeof( ChemDB::Bond::ORDER ) * p_sys.bondOrders.size();
		out += sizeof( Index ) * p_sys.bondPairAtomIndexes.size();

		// Util::Math::RangeList<T> objects have built in static + dynamic size measurement method. So to get dynamic
		// size, we need to remove the static one.
		out += p_sys.atomSolvents.currentSize() - sizeof( Util::Math::RangeList<Index> );
		out += p_sys.atomIons.currentSize() - sizeof( Util::Math::RangeList<Index> );

		return out;
	}

} // namespace VTX::Core::Struct
