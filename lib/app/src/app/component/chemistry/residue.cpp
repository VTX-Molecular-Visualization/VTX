#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include <algorithm>

namespace VTX::App::Component::Chemistry
{
	bool Residue::checkIfStandardFromName( const std::string & p_residueSymbol )
	{
		std::string residueSymbol = p_residueSymbol;
		std::transform( residueSymbol.begin(),
						residueSymbol.end(),
						residueSymbol.begin(),
						[]( unsigned char c ) { return std::toupper( c ); } );

		return std::find(
				   std::begin( ChemDB::Residue::SYMBOL_STR ), std::end( ChemDB::Residue::SYMBOL_STR ), residueSymbol )
			   != std::end( ChemDB::Residue::SYMBOL_STR );
	}

	Molecule * const Residue::getMoleculePtr() const { return _chainPtr->getMoleculePtr(); };
	void			 Residue::setChainPtr( Chain * const p_chain )
	{
		_chainPtr = p_chain;
		initBaseRepresentable( this, _chainPtr, p_chain->getMoleculePtr() );
	}

	const std::string & Residue::getSymbolStr() const
	{
		return _symbol < SYMBOL_COUNT
				   ? ChemDB::Residue::SYMBOL_STR[ _symbol ]
				   : getMoleculePtr()->getUnknownResidueSymbols()[ _symbol - SYMBOL_COUNT ]->symbolStr;
	}
	const std::string & Residue::getSymbolName() const
	{
		return _symbol < SYMBOL_COUNT
				   ? ChemDB::Residue::SYMBOL_NAME[ _symbol ]
				   : getMoleculePtr()->getUnknownResidueSymbols()[ _symbol - SYMBOL_COUNT ]->symbolName;
	}

	const std::string & Residue::getSymbolShort() const
	{
		return _symbol < SYMBOL_COUNT ? ChemDB::Residue::SYMBOL_SHORT_STR[ _symbol ]
									  : ChemDB::Residue::SYMBOL_SHORT_STR[ int( ChemDB::Residue::SYMBOL::UNKNOWN ) ];
	}
	void Residue::setSymbol( const ChemDB::Residue::SYMBOL & p_symbol ) { setSymbol( int( p_symbol ) ); }
	void Residue::setSymbol( const int p_symbolValue )
	{
		_symbol = p_symbolValue;
		BaseModel::setDefaultName( &getSymbolName() );
	};

	void Residue::setAtomCount( const uint p_count )
	{
		_atomCount	   = p_count;
		_realAtomCount = p_count;
	}
	void Residue::removeToAtoms( const uint p_atomIndex )
	{
		const Chemistry::Molecule * const moleculePtr = getMoleculePtr();
		if ( p_atomIndex == _indexFirstAtom )
		{
			while ( _atomCount > 0 && moleculePtr->getAtom( _indexFirstAtom ) == nullptr )
			{
				_indexFirstAtom++;
				_atomCount--;
			}
		}
		else
		{
			uint lastResidueIndex = _indexFirstAtom + _atomCount - 1;
			if ( lastResidueIndex == p_atomIndex )
			{
				while ( _atomCount > 0 && moleculePtr->getAtom( lastResidueIndex ) == nullptr )
				{
					_atomCount--;
					lastResidueIndex--;
				}
			}
		}

		_realAtomCount--;
	}

	const Atom * const Residue::findFirstAtomByName( const std::string & p_name ) const
	{
		for ( uint i = 0; i < _atomCount; ++i )
		{
			const Atom * const atom = getMoleculePtr()->getAtom( _indexFirstAtom + i );
			if ( atom != nullptr && atom->getName() == p_name )
			{
				return atom;
			}
		}

		return nullptr;
	}

	const uint Residue::findBondIndex( const uint p_firstAtomIndex, const uint p_secondAtomIndex ) const
	{
		const uint lastBondIndex = getIndexFirstBond() + getBondCount() - 1;

		// Start from the end because this function will mostly be call to find external bonds that are at the end
		// of bond range.
		for ( uint i = lastBondIndex; i >= getIndexFirstBond(); i-- )
		{
			const Bond * const bond = getMoleculePtr()->getBond( i );

			if ( bond == nullptr )
				continue;

			if ( bond->getIndexFirstAtom() == p_firstAtomIndex && bond->getIndexSecondAtom() == p_secondAtomIndex )
				return i;
		}

		return INVALID_ID;
	}

	const Atom * const Residue::getAlphaCarbon() const { return findFirstAtomByName( "CA" ); }

	void Residue::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Event::Model::RESIDUE_VISIBILITY, _index );
			getMoleculePtr()->propagateEventToViews<uint>( App::Event::Model::RESIDUE_VISIBILITY, _index );
		}
	}

	void Residue::setVisible( const bool p_visible, const bool p_notify )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			if ( p_notify )
			{
				_notifyViews<uint>( App::Event::Model::RESIDUE_VISIBILITY, _index );
				getMoleculePtr()->propagateEventToViews<uint>( App::Event::Model::RESIDUE_VISIBILITY, _index );
			}
		}
	}

 App::Component::Object3D::Helper::AABB Residue::getAABB() const
	{
	 App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB();

		for ( uint i = 0; i < _atomCount; ++i )
		{
			const Atom * const atom = getMoleculePtr()->getAtom( _indexFirstAtom + i );

			if ( atom == nullptr )
				continue;

			aabb.extend( atom->getAABB() );
		}

		return aabb;
	}

 App::Component::Object3D::Helper::AABB Residue::getWorldAABB() const
	{
		const App::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const App::Internal::Math::Transform &		 transform = getMoleculePtr()->getTransform();

	 App::Component::Object3D::Helper::AABB worldAabb   = App::Component::Object3D::Helper::AABB();
		std::vector<Vec3f>	   aabbSummits = aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
			worldAabb.extend( worldSummit );
		}

		return worldAabb;
	}

	void Residue::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }

	const Color::Rgba Residue::getResidueColor( const Chemistry::Residue & p_residue )
	{
		return p_residue.isStandardResidue() ? ChemDB::Residue::SYMBOL_COLOR[ p_residue._symbol ]
											 : ChemDB::Residue::SYMBOL_COLOR[ int( ChemDB::Residue::SYMBOL::UNKNOWN ) ];
	}
} // namespace VTX::App::Component::Chemistry
