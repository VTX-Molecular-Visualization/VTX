#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/core/event/vtx_event.hpp"
#include <algorithm>

namespace VTX::App::Component::Chemistry
{
	void Category::setMoleculePtr( Molecule * const p_molecule )
	{
		_moleculePtr = p_molecule;
		initBaseRepresentable( this, p_molecule, p_molecule );
	}

	void Category::addChain( const uint p_chainIndex ) { _linkedChains.emplace_back( p_chainIndex ); };
	void Category::removeChain( const uint p_chainIndex )
	{
		const std::vector<uint>::const_iterator it
			= std::find( _linkedChains.begin(), _linkedChains.end(), p_chainIndex );

		if ( it != _linkedChains.end() )
			_linkedChains.erase( it );
	};

	const std::vector<uint> & Category::getChains() const { return _linkedChains; };

	bool Category::isEmpty() const
	{
		for ( const uint chainIndex : _linkedChains )
		{
			const Chemistry::Chain * const chain = _moleculePtr->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			if ( chain->getRealResidueCount() > 0 )
				return false;
		}

		return true;
	};
	bool Category::hasChain( const uint p_chainIndex ) const
	{
		return std::find( _linkedChains.begin(), _linkedChains.end(), p_chainIndex ) != _linkedChains.end();
	}
	bool Category::hasResidue( const uint p_residueIndex ) const
	{
		const Chemistry::Residue * const residue = _moleculePtr->getResidue( p_residueIndex );

		if ( residue == nullptr )
			return false;

		return hasChain( residue->getChainPtr()->getIndex() );
	}
	bool Category::hasAtom( const uint p_atomIndex ) const
	{
		const Chemistry::Atom * const atom = _moleculePtr->getAtom( p_atomIndex );

		if ( atom == nullptr )
			return false;

		return hasChain( atom->getChainPtr()->getIndex() );
	}

	int Category::getChainInnerIndex( const uint p_chainIndex ) const
	{
		for ( uint i = 0; i < _linkedChains.size(); i++ )
		{
			if ( _linkedChains[ i ] == p_chainIndex )
				return i;
		}

		return -1;
	}

	std::vector<uint> Category::generateAtomIndexList() const
	{
		std::vector<std::pair<uint, uint>> firstLastAtoms = std::vector<std::pair<uint, uint>>();
		firstLastAtoms.reserve( _linkedChains.size() );

		size_t atomCount = 0;

		for ( const uint chainIndex : _linkedChains )
		{
			const Chemistry::Chain * chain = _moleculePtr->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			const Chemistry::Residue * const firstResidue = _moleculePtr->getResidue( chain->getIndexFirstResidue() );
			const Chemistry::Residue * const lastResidue  = _moleculePtr->getResidue( chain->getIndexLastResidue() );

			const uint firstAtomIndex = firstResidue->getIndexFirstAtom();
			const uint lastAtomIndex  = lastResidue->getIndexFirstAtom() + lastResidue->getAtomCount() - 1;

			firstLastAtoms.emplace_back( std::pair( firstAtomIndex, lastAtomIndex ) );
			atomCount += lastAtomIndex - firstAtomIndex + 1;
		}

		std::vector<uint> atomIndexList = std::vector<uint>();
		atomIndexList.resize( atomCount );

		size_t counter = 0;

		for ( const std::pair<uint, uint> & firstLastAtom : firstLastAtoms )
		{
			for ( uint i = firstLastAtom.first; i <= firstLastAtom.second; i++ )
			{
				atomIndexList[ counter ] = i;
				counter++;
			}
		}

		return atomIndexList;
	}

	void Category::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews<ChemDB::Category::TYPE>( App::Event::Model::CATEGORY_VISIBILITY, _category );
			_moleculePtr->propagateEventToViews<ChemDB::Category::TYPE>( App::Event::Model::CATEGORY_VISIBILITY,
																		 _category );
		}
	}
	void Category::setVisible( const bool p_visible, const bool p_notify )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			if ( p_notify )
			{
				_notifyViews<ChemDB::Category::TYPE>( App::Event::Model::CATEGORY_VISIBILITY, _category );
				_moleculePtr->propagateEventToViews<ChemDB::Category::TYPE>( App::Event::Model::CATEGORY_VISIBILITY,
																			 _category );
			}
		}
	}

	void Category::updateVisibilityState()
	{
		for ( const uint chainIndex : _linkedChains )
		{
			const Chemistry::Chain * const chain = _moleculePtr->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			if ( chain->isVisible() )
			{
				setVisible( true );
				return;
			}
		}

		setVisible( false );
	}

	const App::Component::Object3D::Helper::AABB Category::getAABB() const
	{
	 App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB();

		for ( const uint chainIndex : _linkedChains )
		{
			const Chemistry::Chain * const chain = _moleculePtr->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			aabb.extend( chain->getAABB() );
		}

		return aabb;
	}
	const App::Component::Object3D::Helper::AABB Category::getWorldAABB() const
	{
		const App::Component::Object3D::Helper::AABB aabb	   = getAABB();
		const Math::Transform &		 transform = getMoleculePtr()->getTransform();

	 App::Component::Object3D::Helper::AABB worldAabb   = App::Component::Object3D::Helper::AABB();
		std::vector<Vec3f>	   aabbSummits = aabb.getSummits();

		for ( const Vec3f & summit : aabbSummits )
		{
			const Vec4f worldSummit = transform.get() * Vec4f( summit, 1 );
			worldAabb.extend( worldSummit );
		}

		return worldAabb;
	}

	void Category::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }

} // namespace VTX::App::Component::Chemistry
