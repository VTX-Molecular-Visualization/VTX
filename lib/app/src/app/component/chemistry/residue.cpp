#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/system.hpp"
#include <util/math/range.hpp>

namespace VTX::App::Component::Chemistry
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

	atom_index_t Residue::getIndexFirstAtom() const
	{
		return _moleculePtr->_moleculeStruct.residueFirstAtomIndexes[ _index ];
	}
	void Residue::setIndexFirstAtom( const atom_index_t p_indexFirstAtom )
	{
		_moleculePtr->_moleculeStruct.residueFirstAtomIndexes[ _index ] = atom_index_t( p_indexFirstAtom );
	}
	atom_index_t Residue::getIndexLastAtom() const { return getIndexFirstAtom() + getAtomCount() - 1; }

	atom_index_t Residue::getAtomCount() const { return _moleculePtr->_moleculeStruct.residueAtomCounts[ _index ]; }
	void		 Residue::setAtomCount( const atom_index_t p_atomCount )
	{
		_moleculePtr->_moleculeStruct.residueAtomCounts[ _index ] = p_atomCount;
	}

	size_t Residue::getIndexFirstBond() const
	{
		return _moleculePtr->_moleculeStruct.residueFirstBondIndexes[ _index ];
	}
	void Residue::setIndexFirstBond( const size_t p_indexFirstBond )
	{
		_moleculePtr->_moleculeStruct.residueFirstBondIndexes[ _index ] = p_indexFirstBond;
	}

	size_t Residue::getBondCount() const { return _moleculePtr->_moleculeStruct.residueBondCounts[ _index ]; }
	void   Residue::setBondCount( const size_t p_bondCount )
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

	size_t Residue::getIndexInOriginalChain() const
	{
		return _moleculePtr->_moleculeStruct.residueOriginalIds[ _index ];
	}
	void Residue::setIndexInOriginalChain( const size_t p_index )
	{
		_moleculePtr->_moleculeStruct.residueOriginalIds[ _index ] = p_index;
	}

	const std::string_view Residue::getShortName() const
	{
		ChemDB::Residue::SYMBOL symbol = getSymbol();
		if ( symbol == ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			return _moleculePtr->_moleculeStruct.residueUnknownNames[ _index ];
		}
		else
		{
			return ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ];
		}
	}
	const std::string_view Residue::getName() const
	{
		ChemDB::Residue::SYMBOL symbol = getSymbol();
		if ( symbol == ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			return _moleculePtr->_moleculeStruct.residueUnknownNames[ _index ];
		}
		else
		{
			return ChemDB::Residue::SYMBOL_STR[ int( symbol ) ];
		}
	}

	const std::string_view Residue::getLongName() const
	{
		ChemDB::Residue::SYMBOL symbol = getSymbol();
		if ( symbol == ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			return _moleculePtr->_moleculeStruct.residueUnknownNames[ _index ];
		}
		else
		{
			return ChemDB::Residue::SYMBOL_NAME[ int( symbol ) ];
		}
	}

	ChemDB::Atom::TYPE Residue::getAtomType() const
	{
		Util::Math::Range<atom_index_t> atomRange
			= Util::Math::Range<atom_index_t>( getIndexFirstAtom(), getAtomCount() );

		if ( _moleculePtr->_moleculeStruct.atomSolvents.contains( atomRange ) )
			return ChemDB::Atom::TYPE::SOLVENT;
		else if ( _moleculePtr->_moleculeStruct.atomIons.contains( atomRange ) )
			return ChemDB::Atom::TYPE::ION;

		return ChemDB::Atom::TYPE::NORMAL;
	}
	void Residue::setAtomType( const ChemDB::Atom::TYPE p_type )
	{
		Util::Math::Range<atom_index_t> atomRange
			= Util::Math::Range<atom_index_t>( getIndexFirstAtom(), getAtomCount() );

		switch ( p_type )
		{
		case ChemDB::Atom::TYPE::SOLVENT:
			_moleculePtr->_moleculeStruct.atomSolvents.addRange( atomRange );
			_moleculePtr->_moleculeStruct.atomIons.removeRange( atomRange );
			break;
		case ChemDB::Atom::TYPE::ION:
			_moleculePtr->_moleculeStruct.atomSolvents.removeRange( atomRange );
			_moleculePtr->_moleculeStruct.atomIons.addRange( atomRange );
			break;
		case ChemDB::Atom::TYPE::NORMAL:
			_moleculePtr->_moleculeStruct.atomSolvents.removeRange( atomRange );
			_moleculePtr->_moleculeStruct.atomIons.removeRange( atomRange );
			break;
		}
	}

	AtomIndexRange Residue::getAtomRange() const { return AtomIndexRange( getIndexFirstAtom(), getAtomCount() ); }

	Iterator::AtomContainer Residue::atoms() const
	{
		return Iterator::AtomContainer( _moleculePtr, getIndexFirstAtom(), getAtomCount() );
	}

	bool Residue::isVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange( getIndexFirstAtom(), getAtomCount() );
		return _moleculePtr->_visibleAtomIds.intersectWith( atomRange );
	}
	bool Residue::isFullyVisible() const
	{
		const AtomIndexRange atomRange = AtomIndexRange( getIndexFirstAtom(), getAtomCount() );
		return _moleculePtr->_visibleAtomIds.contains( atomRange );
	}

	void Residue::setVisible( const bool p_visible )
	{
		const AtomIndexRange atomRange = AtomIndexRange( getIndexFirstAtom(), getAtomCount() );
		_moleculePtr->setVisible( atomRange, p_visible );
	}

	void Residue::remove()
	{
		const AtomIndexRange atomRange = AtomIndexRange( getIndexFirstAtom(), getAtomCount() );
		_moleculePtr->remove( atomRange );
	}

} // namespace VTX::App::Component::Chemistry
