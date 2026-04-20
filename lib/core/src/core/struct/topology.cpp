#include "core/struct/topology.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include <util/constants.hpp>

namespace VTX::Core::Struct
{
	void Topology::appendNewChain()
	{
		chainNames.emplace_back( "" );
		chainFirstResidues.emplace_back( INVALID_INDEX );
		chainResidueCounts.emplace_back( 0 );
	}

	void Topology::initChains( const Index p_count )
	{
		chainNames.resize( p_count );
		chainFirstResidues.resize( p_count, INVALID_INDEX );
		chainResidueCounts.resize( p_count, 0 );
	}

	void Topology::initResidues( const Index p_count )
	{
		residueSymbols.resize( p_count, ChemDB::Residue::SYMBOL::UNKNOWN );
		residueChainIndexes.resize( p_count, INVALID_INDEX );
		residueFirstAtomIndexes.resize( p_count, INVALID_INDEX );
		residueAtomCounts.resize( p_count, 0 );
		residueFirstBondIndexes.resize( p_count, INVALID_INDEX );
		residueBondCounts.resize( p_count, 0 );
		residueOriginalIds.resize( p_count, 0 );
		residueSecondaryStructureTypes.resize( p_count, ChemDB::SecondaryStructure::TYPE::UNKNOWN );
		residueNames.resize( p_count );
		residueCategories.resize( p_count, ChemDB::Category::TYPE::UNKNOWN );
	}

	void Topology::initAtoms( const Index p_count )
	{
		atomSymbols.resize( p_count, ChemDB::Atom::SYMBOL::UNKNOWN );
		atomResidueIndexes.resize( p_count, INVALID_INDEX );
		atomNames.resize( p_count );
	}

	void Topology::initBonds( const Index p_count )
	{
		bondOrders.resize( p_count, ChemDB::Bond::ORDER::UNKNOWN );
		bondPairAtomIndexes.resize( p_count * 2, INVALID_INDEX );
	}

	ByteNumber dynamicMemoryUsage( const Topology & p_sys ) noexcept
	{
		ByteNumber out = 0;
		// Mat4f should have dynamic allocation, right ?
		// out += dynamicMemoryUsage( p_sys.trajectory );
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
		out += sizeof( std::string ) * p_sys.residueNames.size();
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
