#include "core/old/struct/chain.hpp"
#include "core/chemdb/chain.hpp"
#include "core/old/struct/molecule.hpp"
#include "core/old/struct/residue.hpp"

namespace VTX::Core::Old::Struct
{
	Util::Color::Rgba Chain::getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm )
	{
		if ( p_chainId.empty() )
			return ChemDB::Chain::CHAIN_ID_UNKNOWN_COLOR;

		// chain id should be defined by one char
		const char c = static_cast<char>( std::toupper( static_cast<unsigned char>( p_chainId[ 0 ] ) ) );

		const int id = int( c ) - 65; // 65 is A
		if ( id < 0 || id > 26 )
			return ChemDB::Chain::CHAIN_ID_UNKNOWN_COLOR;

		return p_isHetAtm ? ChemDB::Chain::CHAIN_ID_COLOR_HETATM[ id ] : ChemDB::Chain::CHAIN_ID_COLOR_ATOM[ id ];
	}

	void Chain::setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

	void Chain::setResidueCount( const uint p_count )
	{
		_residueCount	  = p_count;
		_realResidueCount = p_count;
	}
	void Chain::removeToResidue( const uint p_residueIndex )
	{
		if ( _indexFirstResidue == p_residueIndex )
		{
			while ( _residueCount > 0 && getMoleculePtr()->getResidue( _indexFirstResidue ) == nullptr )
			{
				_indexFirstResidue++;
				_residueCount--;
			}
		}
		else
		{
			uint lastResidueIndex = _indexFirstResidue + _residueCount - 1;
			if ( lastResidueIndex == p_residueIndex )
			{
				while ( _residueCount > 0 && getMoleculePtr()->getResidue( lastResidueIndex ) == nullptr )
				{
					_residueCount--;
					lastResidueIndex--;
				}
			}
		}

		_realResidueCount--;
	}

	uint Chain::computeRealAtomCount() const
	{
		uint realAtomCount = 0;

		for ( uint i = getIndexFirstResidue(); i <= getIndexLastResidue(); i++ )
		{
			const Residue * const residue = _moleculePtr->getResidue( i );

			if ( residue == nullptr )
				continue;

			realAtomCount += residue->getRealAtomCount();
		}

		return realAtomCount;
	}

} // namespace VTX::Core::Old::Struct
