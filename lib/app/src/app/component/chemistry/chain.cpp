#include "app/component/chemistry/chain.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include <core/chemdb/chain.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	Util::Color::Rgba Chain::getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm )
	{
		return VTX::Core::Struct::Chain::getChainIdColor( p_chainId, p_isHetAtm );
	}

	void Chain::setMoleculePtr( Molecule * const p_molecule )
	{
		_moleculePtr = p_molecule;
		_chainStruct->setMoleculePtr( &_moleculePtr->getMoleculeStruct() );

		initBaseRepresentable( this, p_molecule, p_molecule );
	}
	void Chain::setName( const std::string & p_name )
	{
		_chainStruct->setName( p_name );
		BaseModel::setDefaultName( &_chainStruct->getName() );
	}

	void Chain::setCategoryEnum( const ChemDB::Category::TYPE & p_categoryEnum )
	{
		if ( _categoryEnum != p_categoryEnum )
		{
			Chemistry::Molecule * const moleculePtr = getMoleculePtr();
			if ( moleculePtr != nullptr )
			{
				moleculePtr->getCategory( _categoryEnum ).removeChain( _chainStruct->getIndex() );
				_categoryEnum = p_categoryEnum;
				moleculePtr->getCategory( _categoryEnum ).addChain( _chainStruct->getIndex() );
			}
		}
	}

	void Chain::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _chainStruct->getIndex() );
			_moleculePtr->propagateEventToViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _chainStruct->getIndex() );
		}
	}

	void Chain::setVisible( const bool p_visible, const bool p_notify )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			if ( p_notify )
			{
				_notifyViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _chainStruct->getIndex() );
				_moleculePtr->propagateEventToViews<uint>( App::Event::Model::CHAIN_VISIBILITY,
														   _chainStruct->getIndex() );
			}
		}
	}

	const App::Component::Object3D::Helper::AABB Chain::getAABB() const
	{
		App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB();

		for ( uint i = _chainStruct->getIndexFirstResidue(); i <= _chainStruct->getIndexLastResidue(); ++i )
		{
			const Residue * const residue = _moleculePtr->getResidue( i );

			if ( residue == nullptr )
				continue;

			aabb.extend( residue->getAABB() );
		}

		return aabb;
	}
	const App::Component::Object3D::Helper::AABB Chain::getWorldAABB() const
	{
		const App::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const App::Internal::Math::Transform &		 transform = getMoleculePtr()->getTransform();

		App::Component::Object3D::Helper::AABB worldAabb   = App::Component::Object3D::Helper::AABB();
		std::vector<Vec3f>					   aabbSummits = aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
			worldAabb.extend( worldSummit );
		}

		return worldAabb;
	}

	void Chain::removeChildrenRepresentations()
	{
		for ( uint i = _chainStruct->getIndexFirstResidue(); i <= _chainStruct->getIndexLastResidue(); i++ )
		{
			Chemistry::Residue * const residue = getMoleculePtr()->getResidue( i );

			if ( residue == nullptr )
				continue;

			App::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
				*residue, false, false );
		}

		_callRepresentationChange();
	}

	void Chain::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }
} // namespace VTX::App::Component::Chemistry
