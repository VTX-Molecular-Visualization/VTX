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
} // namespace VTX::App::Component::Chemistry
