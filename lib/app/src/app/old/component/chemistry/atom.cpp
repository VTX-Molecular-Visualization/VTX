#include "app/old/component/chemistry/atom.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/chemistry/residue.hpp"

namespace VTX::App::Old::Component::Chemistry
{
	void Atom::setResiduePtr( Residue * const p_residue )
	{
		_residuePtr = p_residue;
		_atomStruct->setResiduePtr( &p_residue->getResidueStruct() );
	}

	Molecule * const Atom::getMoleculePtr() const { return _residuePtr->getChainPtr()->getMoleculePtr(); };
	Chain * const	 Atom::getChainPtr() const { return _residuePtr->getChainPtr(); }

	void Atom::setSymbol( const ChemDB::Atom::SYMBOL p_symbol )
	{
		_atomStruct->setSymbol( p_symbol );
		BaseModel::setDefaultName( &getSymbolName() );
	};

	void Atom::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Old::Event::Model::ATOM_VISIBILITY, _atomStruct->getIndex() );
			getMoleculePtr()->propagateEventToViews<uint>( App::Old::Event::Model::ATOM_VISIBILITY,
														   _atomStruct->getIndex() );
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
				_notifyViews<uint>( App::Old::Event::Model::ATOM_VISIBILITY, _atomStruct->getIndex() );
				getMoleculePtr()->propagateEventToViews<uint>( App::Old::Event::Model::ATOM_VISIBILITY,
															   _atomStruct->getIndex() );
			}
		}
	}

	const Vec3f & Atom::getLocalPosition() const
	{
		return getMoleculePtr()->getAtomPositionFrame( getMoleculePtr()->getFrame() )[ _atomStruct->getIndex() ];
	}

	const Vec3f Atom::getWorldPosition() const
	{
		const Vec3f &						   localPosition = getLocalPosition();
		const App::Old::Internal::Math::Transform & transform	 = getMoleculePtr()->getTransform();

		const Vec4f worldPosition = transform.get() * Vec4f( localPosition, 1 );
		return Vec3f( worldPosition.x, worldPosition.y, worldPosition.z );
	}
	const App::Old::Component::Object3D::Helper::AABB Atom::getAABB() const
	{
		Vec3f & position
			= getMoleculePtr()->getAtomPositionFrame( getMoleculePtr()->getFrame() )[ _atomStruct->getIndex() ];
		App::Old::Component::Object3D::Helper::AABB aabb
			= App::Old::Component::Object3D::Helper::AABB( position, getVdwRadius() );

		return aabb;
	}
	const App::Old::Component::Object3D::Helper::AABB Atom::getWorldAABB() const
	{
		const App::Old::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const App::Old::Internal::Math::Transform &		 transform = getMoleculePtr()->getTransform();

		const Vec4f worldPosition = transform.get() * Vec4f( aabb.centroid(), 1 );

		return App::Old::Component::Object3D::Helper::AABB( Vec3f( worldPosition ), getVdwRadius() );
	}
} // namespace VTX::App::Old::Component::Chemistry
