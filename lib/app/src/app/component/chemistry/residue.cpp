#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/system.hpp"
#include <util/math/range.hpp>

namespace VTX::App::Component::Chemistry
{
	const Chain * const Residue::getConstChainPtr() const
	{
		return _systemPtr->getChain( _systemPtr->_systemStruct.residueChainIndexes[ _index ] );
	}

	Chain * const Residue::getChainPtr() const
	{
		return _systemPtr->getChain( _systemPtr->_systemStruct.residueChainIndexes[ _index ] );
	}

	void Residue::setChainPtr( Chain * const p_chainPtr )
	{
		_systemPtr->_systemStruct.residueChainIndexes[ _index ] = p_chainPtr->getIndex();
	}

	Index Residue::getIndexFirstAtom() const { return _systemPtr->_systemStruct.residueFirstAtomIndexes[ _index ]; }

	void Residue::setIndexFirstAtom( const Index p_indexFirstAtom )
	{
		_systemPtr->_systemStruct.residueFirstAtomIndexes[ _index ] = Index( p_indexFirstAtom );
	}

	Index Residue::getIndexLastAtom() const { return getIndexFirstAtom() + getAtomCount() - 1; }

	Index Residue::getAtomCount() const { return _systemPtr->_systemStruct.residueAtomCounts[ _index ]; }

	void Residue::setAtomCount( const Index p_atomCount )
	{
		_systemPtr->_systemStruct.residueAtomCounts[ _index ] = p_atomCount;
	}

	Index Residue::getIndexFirstBond() const { return _systemPtr->_systemStruct.residueFirstBondIndexes[ _index ]; }

	void Residue::setIndexFirstBond( const Index p_indexFirstBond )
	{
		_systemPtr->_systemStruct.residueFirstBondIndexes[ _index ] = p_indexFirstBond;
	}

	Index Residue::getBondCount() const { return _systemPtr->_systemStruct.residueBondCounts[ _index ]; }

	void Residue::setBondCount( const Index p_bondCount )
	{
		_systemPtr->_systemStruct.residueBondCounts[ _index ] = p_bondCount;
	}

	ChemDB::Residue::SYMBOL Residue::getSymbol() const { return _systemPtr->_systemStruct.residueSymbols[ _index ]; }
	void					Residue::setSymbol( const ChemDB::Residue::SYMBOL p_symbol )
	{
		_systemPtr->_systemStruct.residueSymbols[ _index ] = p_symbol;
	}

	Index Residue::getIndexInOriginalChain() const { return _systemPtr->_systemStruct.residueOriginalIds[ _index ]; }

	void Residue::setIndexInOriginalChain( const Index p_index )
	{
		_systemPtr->_systemStruct.residueOriginalIds[ _index ] = p_index;
	}

	const std::string_view Residue::getShortName() const
	{
		ChemDB::Residue::SYMBOL symbol = getSymbol();
		if ( symbol == ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			return _systemPtr->_systemStruct.residueUnknownNames[ _index ];
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
			return _systemPtr->_systemStruct.residueUnknownNames[ _index ];
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
			return _systemPtr->_systemStruct.residueUnknownNames[ _index ];
		}
		else
		{
			return ChemDB::Residue::SYMBOL_NAME[ int( symbol ) ];
		}
	}

	ChemDB::Atom::TYPE Residue::getAtomType() const
	{
		Util::Math::Range<Index> atomRange = Util::Math::Range<Index>( getIndexFirstAtom(), getAtomCount() );

		if ( _systemPtr->_systemStruct.atomSolvents.contains( atomRange ) )
			return ChemDB::Atom::TYPE::SOLVENT;
		else if ( _systemPtr->_systemStruct.atomIons.contains( atomRange ) )
			return ChemDB::Atom::TYPE::ION;

		return ChemDB::Atom::TYPE::NORMAL;
	}

	void Residue::setAtomType( const ChemDB::Atom::TYPE p_type )
	{
		Util::Math::Range<Index> atomRange = Util::Math::Range<Index>( getIndexFirstAtom(), getAtomCount() );

		switch ( p_type )
		{
		case ChemDB::Atom::TYPE::SOLVENT:
			_systemPtr->_systemStruct.atomSolvents.addRange( atomRange );
			_systemPtr->_systemStruct.atomIons.removeRange( atomRange );
			break;
		case ChemDB::Atom::TYPE::ION:
			_systemPtr->_systemStruct.atomSolvents.removeRange( atomRange );
			_systemPtr->_systemStruct.atomIons.addRange( atomRange );
			break;
		case ChemDB::Atom::TYPE::NORMAL:
			_systemPtr->_systemStruct.atomSolvents.removeRange( atomRange );
			_systemPtr->_systemStruct.atomIons.removeRange( atomRange );
			break;
		}
	}

	IndexRange Residue::getAtomRange() const { return IndexRange( getIndexFirstAtom(), getAtomCount() ); }

	Iterator::AtomContainer Residue::atoms() const
	{
		return Iterator::AtomContainer( _systemPtr, getIndexFirstAtom(), getAtomCount() );
	}

	bool Residue::isVisible() const
	{
		const IndexRange atomRange = IndexRange( getIndexFirstAtom(), getAtomCount() );
		return _systemPtr->_visibleAtomIds.intersectWith( atomRange );
	}

	bool Residue::isFullyVisible() const
	{
		const IndexRange atomRange = IndexRange( getIndexFirstAtom(), getAtomCount() );
		return _systemPtr->_visibleAtomIds.contains( atomRange );
	}

	void Residue::setVisible( const bool p_visible )
	{
		const IndexRange atomRange = IndexRange( getIndexFirstAtom(), getAtomCount() );
		_systemPtr->setVisible( atomRange, p_visible );
	}

	void Residue::remove()
	{
		const IndexRange atomRange = IndexRange( getIndexFirstAtom(), getAtomCount() );
		_systemPtr->remove( atomRange );
	}

} // namespace VTX::App::Component::Chemistry
