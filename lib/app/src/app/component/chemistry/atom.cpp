#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"

namespace VTX::App::Component::Chemistry
{
	Molecule * const Atom::getMoleculePtr() const { return _residuePtr->getChainPtr()->getMoleculePtr(); };
	Chain * const	 Atom::getChainPtr() const { return _residuePtr->getChainPtr(); }

	void Atom::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Event::Model::ATOM_VISIBILITY, _index );
			getMoleculePtr()->propagateEventToViews<uint>( App::Event::Model::ATOM_VISIBILITY, _index );
		}
	}
	void Atom::setVisible( const bool p_visible, const bool p_notify )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			if ( p_notify )
			{
				_notifyViews<uint>( App::Event::Model::ATOM_VISIBILITY, _index );
				getMoleculePtr()->propagateEventToViews<uint>( App::Event::Model::ATOM_VISIBILITY, _index );
			}
		}
	}

	const Vec3f & Atom::getLocalPosition() const
	{
		return getMoleculePtr()->getAtomPositionFrame( getMoleculePtr()->getFrame() )[ _index ];
	}

	const Vec3f Atom::getWorldPosition() const
	{
		const Vec3f &			localPosition = getLocalPosition();
		const Math::Transform & transform	  = getMoleculePtr()->getTransform();

		const Vec4f worldPosition = transform.get() * Vec4f( localPosition, 1 );
		return Vec3f( worldPosition.x, worldPosition.y, worldPosition.z );
	}
	const App::Component::Object3D::Helper::AABB Atom::getAABB() const
	{
		Vec3f & position			= getMoleculePtr()->getAtomPositionFrame( getMoleculePtr()->getFrame() )[ _index ];
	 App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB( position, getVdwRadius() );

		return aabb;
	}
	const App::Component::Object3D::Helper::AABB Atom::getWorldAABB() const
	{
		const App::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const Math::Transform &		 transform = getMoleculePtr()->getTransform();

		const Vec4f worldPosition = transform.get() * Vec4f( aabb.centroid(), 1 );

		return App::Component::Object3D::Helper::AABB( Vec3f( worldPosition ), getVdwRadius() );
	}
} // namespace VTX::App::Component::Chemistry
