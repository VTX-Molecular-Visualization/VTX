#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::Component::Chemistry
{
	Atom::Atom( Residue * const p_residue ) : Atom( p_residue->getMoleculePtr() ) {};

	Residue * Atom::getResiduePtr() const
	{
		return _moleculePtr->getResidue( _moleculePtr->_moleculeStruct.atomResidueIndexes[ _index ] );
	}
	const Residue * Atom::getConstResiduePtr() const
	{
		return _moleculePtr->getResidue( _moleculePtr->_moleculeStruct.atomResidueIndexes[ _index ] );
	}
	void Atom::setResiduePtr( Residue * p_residue )
	{
		_moleculePtr->_moleculeStruct.atomResidueIndexes[ _index ] = p_residue->getIndex();
	}

	Chain *		  Atom::getChainPtr() const { return getResiduePtr()->getChainPtr(); }
	const Chain * Atom::getConstChainPtr() const { return getConstResiduePtr()->getConstChainPtr(); }

	const std::string & Atom::getName() const { return _moleculePtr->_moleculeStruct.atomNames[ _index ]; }
	void Atom::setName( const std::string & p_name ) { _moleculePtr->_moleculeStruct.atomNames[ _index ] = p_name; }

	float Atom::getVdwRadius() const
	{
		return ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( _moleculePtr->_moleculeStruct.atomSymbols[ _index ] ) ];
	}

	const Vec3f & Atom::getLocalPosition() const
	{
		return getLocalPosition( _moleculePtr->_moleculeStruct.trajectory.currentFrameIndex );
	}
	const Vec3f & Atom::getLocalPosition( const size_t & p_frameIndex ) const
	{
		return _moleculePtr->getTrajectory().frames[ p_frameIndex ][ _index ];
	}
	Vec3f Atom::getWorldPosition() const
	{
		return Vec3f( Vec4f( getLocalPosition(), 0 ) * _moleculePtr->getTransform().get() );
	}
	Vec3f Atom::getWorldPosition( const size_t & p_frameIndex ) const
	{
		return Vec3f( Vec4f( getLocalPosition( p_frameIndex ), 0 ) * _moleculePtr->getTransform().get() );
	}

	const ChemDB::Atom::SYMBOL & Atom::getSymbol() const { return _moleculePtr->_moleculeStruct.atomSymbols[ _index ]; }
	void						 Atom::setSymbol( const ChemDB::Atom::SYMBOL & p_symbol )
	{
		_moleculePtr->_moleculeStruct.atomSymbols[ _index ] = p_symbol;
	}

	ChemDB::Atom::TYPE Atom::getType() const
	{
		if ( _moleculePtr->_moleculeStruct.atomSolvents.contains( atom_index_t( _index ) ) )
			return ChemDB::Atom::TYPE::SOLVENT;
		else if ( _moleculePtr->_moleculeStruct.atomIons.contains( atom_index_t( _index ) ) )
			return ChemDB::Atom::TYPE::ION;

		return ChemDB::Atom::TYPE::NORMAL;
	}
	void Atom::setType( const ChemDB::Atom::TYPE p_type )
	{
		switch ( p_type )
		{
		case ChemDB::Atom::TYPE::SOLVENT:
			_moleculePtr->_moleculeStruct.atomSolvents.addValue( atom_index_t( _index ) );
			_moleculePtr->_moleculeStruct.atomIons.removeValue( atom_index_t( _index ) );
			break;
		case ChemDB::Atom::TYPE::ION:
			_moleculePtr->_moleculeStruct.atomSolvents.removeValue( atom_index_t( _index ) );
			_moleculePtr->_moleculeStruct.atomIons.addValue( atom_index_t( _index ) );
			break;
		case ChemDB::Atom::TYPE::NORMAL:
			_moleculePtr->_moleculeStruct.atomSolvents.removeValue( atom_index_t( _index ) );
			_moleculePtr->_moleculeStruct.atomIons.removeValue( atom_index_t( _index ) );
			break;
		}
	}

	bool Atom::isVisible() const { return _moleculePtr->_visibleAtomIds.contains( _index ); }
	void Atom::setVisible( const bool p_visible ) { _moleculePtr->setVisible( _index, p_visible ); }

	void Atom::remove() { _moleculePtr->remove( _index ); }
} // namespace VTX::App::Component::Chemistry
