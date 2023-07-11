#include "app/component/chemistry/flat/residue.hpp"
#include "app/component/chemistry/flat/chain.hpp"
#include "app/component/chemistry/flat/molecule.hpp"

namespace VTX::App::Component::Chemistry::Flat
{
	const Chain * const Residue::getConstChainPtr() const
	{
		return _moleculePtr->getChain( _moleculePtr->_moleculeStruct.residueChainIndexes[ _index ] );
	}
	Chain * const Residue::getChainPtr() const
	{
		return _moleculePtr->getChain( _moleculePtr->_moleculeStruct.residueChainIndexes[ _index ] );
	}
	void Residue::setChainPtr( Chain * const p_chainPtr )
	{
		_moleculePtr->_moleculeStruct.residueChainIndexes[ _index ] = p_chainPtr->getIndex();
	}

	const size_t Residue::getIndexFirstAtom() const
	{
		return _moleculePtr->_moleculeStruct.residueFirstAtomIndexes[ _index ];
	}
	void Residue::setIndexFirstAtom( const size_t p_indexFirstAtom )
	{
		_moleculePtr->_moleculeStruct.residueFirstAtomIndexes[ _index ] = p_indexFirstAtom;
	}

	const size_t Residue::getAtomCount() const { return _moleculePtr->_moleculeStruct.residueAtomCounts[ _index ]; }
	void		 Residue::setAtomCount( const size_t p_atomCount )
	{
		_moleculePtr->_moleculeStruct.residueAtomCounts[ _index ] = p_atomCount;
	}

	const size_t Residue::getIndexFirstBond() const
	{
		return _moleculePtr->_moleculeStruct.residueFirstBondIndexes[ _index ];
	}
	void Residue::setIndexFirstBond( const size_t p_indexFirstBond )
	{
		_moleculePtr->_moleculeStruct.residueFirstBondIndexes[ _index ] = p_indexFirstBond;
	}

	const size_t Residue::getBondCount() const { return _moleculePtr->_moleculeStruct.residueBondCounts[ _index ]; }
	void		 Residue::setBondCount( const size_t p_bondCount )
	{
		_moleculePtr->_moleculeStruct.residueBondCounts[ _index ] = p_bondCount;
	}

	ChemDB::Residue::SYMBOL Residue::getSymbol() const
	{
		return _moleculePtr->_moleculeStruct.residueSymbols[ _index ];
	}
	void Residue::setSymbol( const ChemDB::Residue::SYMBOL p_symbol )
	{
		_moleculePtr->_moleculeStruct.residueSymbols[ _index ] = p_symbol;
	}

} // namespace VTX::App::Component::Chemistry::Flat
