#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/internal/chemdb/chain.hpp"
#include "app/old_app/representation/representation_manager.hpp"

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = App::Internal::ChemDB;

	void Chain::setMoleculePtr( Molecule * const p_molecule )
	{
		_moleculePtr = p_molecule;

		initBaseRepresentable( this, p_molecule, p_molecule );
	}

	void Chain::setResidueCount( const uint p_count )
	{
		_residueCount	  = p_count;
		_realResidueCount = p_count;
	}
	void Chain::removeToResidues( const uint p_residueIndex )
	{
		if ( _indexFirstResidue == p_residueIndex )
		{
			while ( _residueCount > 0 && getMoleculePtr()->getResidue( _indexFirstResidue ) == nullptr )
			{
				_indexFirstResidue++;
				_residueCount--;
			}
		}
		else
		{
			uint lastResidueIndex = _indexFirstResidue + _residueCount - 1;
			if ( lastResidueIndex == p_residueIndex )
			{
				while ( _residueCount > 0 && getMoleculePtr()->getResidue( lastResidueIndex ) == nullptr )
				{
					_residueCount--;
					lastResidueIndex--;
				}
			}
		}

		_realResidueCount--;
	}

	uint Chain::computeRealAtomCount() const
	{
		uint realAtomCount = 0;

		for ( uint i = getIndexFirstResidue(); i <= getIndexLastResidue(); i++ )
		{
			const Chemistry::Residue * const residue = _moleculePtr->getResidue( i );

			if ( residue == nullptr )
				continue;

			realAtomCount += residue->getRealAtomCount();
		}

		return realAtomCount;
	}

	Color::Rgba Chain::getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm )
	{
		if ( p_chainId.empty() )
			return ChemDB::Chain::CHAIN_ID_UNKNOWN_COLOR;

		// chain id should be defined by one char
		const char c = static_cast<char>( std::toupper( static_cast<unsigned char>( p_chainId[ 0 ] ) ) );

		const int id = int( c ) - 65; // 65 is A
		if ( id < 0 || id > 26 )
			return ChemDB::Chain::CHAIN_ID_UNKNOWN_COLOR;

		return p_isHetAtm ? ChemDB::Chain::CHAIN_ID_COLOR_HETATM[ id ] : ChemDB::Chain::CHAIN_ID_COLOR_ATOM[ id ];
	}

	void Chain::setCategoryEnum( const ChemDB::Category::TYPE & p_categoryEnum )
	{
		if ( _categoryEnum != p_categoryEnum )
		{
			Chemistry::Molecule * const moleculePtr = getMoleculePtr();
			if ( moleculePtr != nullptr )
			{
				moleculePtr->getCategory( _categoryEnum ).removeChain( _index );
				_categoryEnum = p_categoryEnum;
				moleculePtr->getCategory( _categoryEnum ).addChain( _index );
			}
		}
	}

	void Chain::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _index );
			_moleculePtr->propagateEventToViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _index );
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
				_notifyViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _index );
				_moleculePtr->propagateEventToViews<uint>( App::Event::Model::CHAIN_VISIBILITY, _index );
			}
		}
	}

	const Object3D::Helper::AABB Chain::getAABB() const
	{
		Object3D::Helper::AABB aabb = Object3D::Helper::AABB();

		for ( uint i = 0; i < _residueCount; ++i )
		{
			const Residue * const residue = _moleculePtr->getResidue( _indexFirstResidue + i );

			if ( residue == nullptr )
				continue;

			aabb.extend( residue->getAABB() );
		}

		return aabb;
	}
	const Object3D::Helper::AABB Chain::getWorldAABB() const
	{
		const Object3D::Helper::AABB aabb	   = getAABB();
		const Math::Transform &		 transform = getMoleculePtr()->getTransform();

		Object3D::Helper::AABB worldAabb   = Object3D::Helper::AABB();
		std::vector<Vec3f>	   aabbSummits = aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
			worldAabb.extend( worldSummit );
		}

		return worldAabb;
	}

	void Chain::removeChildrenRepresentations()
	{
		for ( uint i = _indexFirstResidue; i < _indexFirstResidue + _residueCount; i++ )
		{
			Chemistry::Residue * const residue = getMoleculePtr()->getResidue( i );

			if ( residue == nullptr )
				continue;

			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
				*residue, false, false );
		}

		_callRepresentationChange();
	}

	void Chain::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }
} // namespace VTX::App::Component::Chemistry
