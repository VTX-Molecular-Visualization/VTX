#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include <core/chemdb/atom.hpp>

namespace VTX::App::Component::Chemistry
{
	Atom::Atom( Residue * const p_residue ) : Atom( p_residue->getMoleculePtr() ) {};

	float Atom::getVdwRadius() const { return _moleculePtr->_atomRadii[ _index ]; }

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

	bool Atom::isVisible() { return _moleculePtr->_atomVisibilities[ _index ]; }
	void Atom::setVisible( const bool p_visible ) { _moleculePtr->_atomVisibilities[ _index ] = p_visible; }

	ChemDB::Atom::TYPE Atom::getType() const
	{
		if ( _moleculePtr->_moleculeStruct.atomSolvents.contains( _index ) )
			return ChemDB::Atom::TYPE::SOLVENT;
		else if ( _moleculePtr->_moleculeStruct.atomIons.contains( _index ) )
			return ChemDB::Atom::TYPE::ION;

		return ChemDB::Atom::TYPE::NORMAL;
	}
	void Atom::setType( const ChemDB::Atom::TYPE p_type )
	{
		switch ( p_type )
		{
		case ChemDB::Atom::TYPE::SOLVENT:
			_moleculePtr->_moleculeStruct.atomSolvents.addValue( _index );
			_moleculePtr->_moleculeStruct.atomIons.removeValue( _index );
			break;
		case ChemDB::Atom::TYPE::ION:
			_moleculePtr->_moleculeStruct.atomSolvents.removeValue( _index );
			_moleculePtr->_moleculeStruct.atomIons.addValue( _index );
			break;
		case ChemDB::Atom::TYPE::NORMAL:
			_moleculePtr->_moleculeStruct.atomSolvents.removeValue( _index );
			_moleculePtr->_moleculeStruct.atomIons.removeValue( _index );
			break;
		}
	}
} // namespace VTX::App::Component::Chemistry
