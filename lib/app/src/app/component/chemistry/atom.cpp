#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::Component::Chemistry
{
	Atom::Atom( Residue * const p_residue ) : Atom( p_residue->getSystemPtr() ) {};

	Residue * Atom::getResiduePtr() const
	{
		return _systemPtr->getResidue( _systemPtr->_systemStruct.atomResidueIndexes[ _index ] );
	}
	const Residue * Atom::getConstResiduePtr() const
	{
		return _systemPtr->getResidue( _systemPtr->_systemStruct.atomResidueIndexes[ _index ] );
	}
	void Atom::setResiduePtr( Residue * p_residue )
	{
		_systemPtr->_systemStruct.atomResidueIndexes[ _index ] = p_residue->getIndex();
	}

	Chain *		  Atom::getChainPtr() const { return getResiduePtr()->getChainPtr(); }
	const Chain * Atom::getConstChainPtr() const { return getConstResiduePtr()->getConstChainPtr(); }

	const std::string & Atom::getName() const { return _systemPtr->_systemStruct.atomNames[ _index ]; }
	void Atom::setName( const std::string & p_name ) { _systemPtr->_systemStruct.atomNames[ _index ] = p_name; }

	float Atom::getVdwRadius() const
	{
		return ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _systemPtr->_systemStruct.atomSymbols[ _index ] ) ];
	}

	const Vec3f & Atom::getLocalPosition() const
	{
		return getLocalPosition( _systemPtr->_systemStruct.trajectory.currentFrameIndex );
	}
	const Vec3f & Atom::getLocalPosition( const size_t & p_frameIndex ) const
	{
		return _systemPtr->getTrajectory().frames[ p_frameIndex ][ _index ];
	}
	Vec3f Atom::getWorldPosition() const
	{
		return Vec3f( Vec4f( getLocalPosition(), 0 ) * _systemPtr->getTransform().get() );
	}
	Vec3f Atom::getWorldPosition( const size_t & p_frameIndex ) const
	{
		return Vec3f( Vec4f( getLocalPosition( p_frameIndex ), 0 ) * _systemPtr->getTransform().get() );
	}

	const ChemDB::Atom::SYMBOL & Atom::getSymbol() const { return _systemPtr->_systemStruct.atomSymbols[ _index ]; }
	void						 Atom::setSymbol( const ChemDB::Atom::SYMBOL & p_symbol )
	{
		_systemPtr->_systemStruct.atomSymbols[ _index ] = p_symbol;
	}

	ChemDB::Atom::TYPE Atom::getType() const
	{
		if ( _systemPtr->_systemStruct.atomSolvents.contains( atom_index_t( _index ) ) )
			return ChemDB::Atom::TYPE::SOLVENT;
		else if ( _systemPtr->_systemStruct.atomIons.contains( atom_index_t( _index ) ) )
			return ChemDB::Atom::TYPE::ION;

		return ChemDB::Atom::TYPE::NORMAL;
	}
	void Atom::setType( const ChemDB::Atom::TYPE p_type )
	{
		switch ( p_type )
		{
		case ChemDB::Atom::TYPE::SOLVENT:
			_systemPtr->_systemStruct.atomSolvents.addValue( atom_index_t( _index ) );
			_systemPtr->_systemStruct.atomIons.removeValue( atom_index_t( _index ) );
			break;
		case ChemDB::Atom::TYPE::ION:
			_systemPtr->_systemStruct.atomSolvents.removeValue( atom_index_t( _index ) );
			_systemPtr->_systemStruct.atomIons.addValue( atom_index_t( _index ) );
			break;
		case ChemDB::Atom::TYPE::NORMAL:
			_systemPtr->_systemStruct.atomSolvents.removeValue( atom_index_t( _index ) );
			_systemPtr->_systemStruct.atomIons.removeValue( atom_index_t( _index ) );
			break;
		}
	}

	bool Atom::isVisible() const { return _systemPtr->_visibleAtomIds.contains( _index ); }
	void Atom::setVisible( const bool p_visible ) { _systemPtr->setVisible( _index, p_visible ); }

	void Atom::remove() { _systemPtr->remove( _index ); }
} // namespace VTX::App::Component::Chemistry
