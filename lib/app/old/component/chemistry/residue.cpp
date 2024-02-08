#include "app/old/component/chemistry/residue.hpp"
#include "app/old/component/chemistry/atom.hpp"
#include "app/old/component/chemistry/bond.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include <algorithm>

namespace VTX::App::Old::Component::Chemistry
{
	bool Residue::checkIfStandardFromName( const std::string & p_residueSymbol )
	{
		return VTX::Core::Old::Struct::Residue::checkIfStandardFromName( p_residueSymbol );
	}
	const Util::Color::Rgba Residue::getResidueColor( const Chemistry::Residue & p_residue )
	{
		return VTX::Core::Old::Struct::Residue::getResidueColor( p_residue.getResidueStruct() );
	}

	Molecule * const Residue::getMoleculePtr() const { return _chainPtr->getMoleculePtr(); };
	void			 Residue::setChainPtr( Chain * const p_chain )
	{
		_chainPtr = p_chain;
		_residueStruct->setChainPtr( &p_chain->getChainStruct() );
		initBaseRepresentable( this, _chainPtr, p_chain->getMoleculePtr() );
	}

	void Residue::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Old::Event::Model::RESIDUE_VISIBILITY, getIndex() );
			getMoleculePtr()->propagateEventToViews<uint>( App::Old::Event::Model::RESIDUE_VISIBILITY, getIndex() );
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
				_notifyViews<uint>( App::Old::Event::Model::RESIDUE_VISIBILITY, getIndex() );
				getMoleculePtr()->propagateEventToViews<uint>( App::Old::Event::Model::RESIDUE_VISIBILITY, getIndex() );
			}
		}
	}

	App::Old::Component::Object3D::Helper::AABB Residue::getAABB() const
	{
		App::Old::Component::Object3D::Helper::AABB aabb = App::Old::Component::Object3D::Helper::AABB();

		for ( uint i = 0; i < getAtomCount(); ++i )
		{
			const Atom * const atom = getMoleculePtr()->getAtom( getIndexFirstAtom() + i );

			if ( atom == nullptr )
				continue;

			aabb.extend( atom->getAABB() );
		}

		return aabb;
	}

	App::Old::Component::Object3D::Helper::AABB Residue::getWorldAABB() const
	{
		const App::Old::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const App::Old::Internal::Math::Transform &		 transform = getMoleculePtr()->getTransform();

		App::Old::Component::Object3D::Helper::AABB worldAabb   = App::Old::Component::Object3D::Helper::AABB();
		std::vector<Vec3f>					   aabbSummits = aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
			worldAabb.extend( worldSummit );
		}

		return worldAabb;
	}

	void Residue::_onRepresentationChange() { _notifyViews( App::Old::Event::Model::REPRESENTATION_CHANGE ); }

} // namespace VTX::App::Old::Component::Chemistry