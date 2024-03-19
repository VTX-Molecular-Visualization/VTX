#include "app/component/chemistry/molecule.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/uid_system.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <util/algorithm/range.hpp>
#include <util/constants.hpp>

namespace
{
	template<typename T>
	static void eraseEndingNullValues( std::vector<T> & p_vector )
	{
		typename std::vector<T>::const_reverse_iterator it = p_vector.crbegin();

		while ( it != p_vector.crend() && ( *it ) == nullptr )
		{
			++it;
		}

		p_vector.erase( it.base(), p_vector.cend() );
	}
} // namespace

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	Molecule::Molecule()
	{
		_chains	  = std::vector<std::unique_ptr<Chemistry::Chain>>();
		_residues = std::vector<std::unique_ptr<Chemistry::Residue>>();
		_atoms	  = std::vector<std::unique_ptr<Chemistry::Atom>>();
		_bonds	  = std::vector<std::unique_ptr<Chemistry::Bond>>();
	};
	Molecule::Molecule( VTX::Core::Struct::Molecule & p_moleculeStruct ) { setMoleculeStruct( p_moleculeStruct ); }
	Molecule::~Molecule()
	{
		if ( _atomUidRange.isValid() )
			UID_SYSTEM().unregister( _atomUidRange );

		if ( _residueUidRange.isValid() )
			UID_SYSTEM().unregister( _residueUidRange );
	};

	void Molecule::setMoleculeStruct( VTX::Core::Struct::Molecule & p_moleculeStruct )
	{
		_moleculeStruct = std::move( p_moleculeStruct );

		initChains( _moleculeStruct.getChainCount() );
		initResidues( _moleculeStruct.getResidueCount() );
		initAtoms( _moleculeStruct.getAtomCount() );
		initBonds( _moleculeStruct.getBondCount() );
	}

	void Molecule::initChains( const size_t p_chainCount )
	{
		_chains.resize( p_chainCount );
		std::generate(
			_chains.begin(),
			_chains.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Chain>( this, n++ ) ); }
		);
	}

	void Molecule::initResidues( const size_t p_residueCount )
	{
		_residues.resize( p_residueCount );
		std::generate(
			_residues.begin(),
			_residues.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Residue>( this, n++ ) ); }
		);
	}

	void Molecule::initAtoms( const size_t p_atomCount )
	{
		_atoms.resize( p_atomCount );
		std::generate(
			_atoms.begin(),
			_atoms.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Atom>( this, n++ ) ); }
		);

		_atomUidRange = UID_SYSTEM().registerRange( Core::UID::uid( p_atomCount ) );
	}

	void Molecule::initBonds( const size_t p_bondCount )
	{
		_bonds.resize( p_bondCount );
		std::generate(
			_bonds.begin(),
			_bonds.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Bond>( this, n++ ) ); }
		);
	}

	void Molecule::setName( const std::string & p_name )
	{
		Component::Scene::SceneItemComponent & sceneComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *this );
		sceneComponent.setName( p_name );
		_moleculeStruct.name = p_name;
	}

	const Atom * Molecule::getAtomFromUID( Core::UID::uid p_uid ) const
	{
		return getAtom( p_uid - _atomUidRange.getFirst() );
	}
	Atom * Molecule::getAtomFromUID( Core::UID::uid p_uid ) { return getAtom( p_uid - _atomUidRange.getFirst() ); }
	const Residue * Molecule::getResidueFromUID( Core::UID::uid p_uid ) const
	{
		return getResidue( p_uid - _residueUidRange.getFirst() );
	}
	Residue * Molecule::getResidueFromUID( Core::UID::uid p_uid )
	{
		return getResidue( p_uid - _residueUidRange.getFirst() );
	}

	bool Molecule::isVisible() const { return !_visibleAtomIds.isEmpty(); }
	bool Molecule::isFullyVisible() const { return _visibleAtomIds.count() == atom_index_t( _atoms.size() ); }

	void Molecule::setVisible( const bool p_visible )
	{
		const AtomIndexRange atomRange = AtomIndexRange( 0, atom_index_t( _atoms.size() ) );

		if ( p_visible )
		{
			_visibleAtomIds.addRange( atomRange );
		}
		else
		{
			// = _visibleAtomIds.removeRange( atomRange ), but quicker
			_visibleAtomIds.clear();
		}

		const App::Core::VISIBILITY_APPLY_MODE applyMode
			= p_visible ? App::Core::VISIBILITY_APPLY_MODE::SHOW : App::Core::VISIBILITY_APPLY_MODE::HIDE;

		onVisibilityChange( AtomIndexRangeList( { atomRange } ), applyMode );
	}
	void Molecule::setVisible( const atom_index_t & p_atomId, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			_visibleAtomIds.addValue( p_atomId );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			_visibleAtomIds.removeValue( p_atomId );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( AtomIndexRangeList( { AtomIndexRange( p_atomId ) } ), applyMode );
	}
	void Molecule::setVisible( const AtomIndexRange & p_atomRange, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			_visibleAtomIds.addRange( p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			_visibleAtomIds.removeRange( p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( AtomIndexRangeList( { p_atomRange } ), applyMode );
	}

	void Molecule::setVisible( const AtomIndexRangeList & p_atomRange, bool p_visible )
	{
		App::Core::VISIBILITY_APPLY_MODE applyMode;

		if ( p_visible )
		{
			Util::Algorithm::Range::mergeInSitu( _visibleAtomIds, p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::SHOW;
		}
		else
		{
			Util::Algorithm::Range::substractInSitu( _visibleAtomIds, p_atomRange );
			applyMode = App::Core::VISIBILITY_APPLY_MODE::HIDE;
		}

		onVisibilityChange( p_atomRange, applyMode );
	}

	void Molecule::setAtomVisibilities( const AtomIndexRangeList & p_visibility )
	{
		_visibleAtomIds = p_visibility;
		onVisibilityChange( _visibleAtomIds, App::Core::VISIBILITY_APPLY_MODE::SET );
	}

	void Molecule::remove( const atom_index_t & p_atomIndex )
	{
		_activeAtomIds.removeValue( p_atomIndex );
		_deleteTopologyPointers( p_atomIndex );
		_resizeTopologyVectors();

		onAtomRemoved( Util::Math::RangeList<atom_index_t>( p_atomIndex ) );
	}
	void Molecule::remove( const AtomIndexRange & p_atomRange )
	{
		_activeAtomIds.removeRange( p_atomRange );
		_deleteTopologyPointers( p_atomRange );
		_resizeTopologyVectors();

		onAtomRemoved( Util::Math::RangeList<atom_index_t>( { p_atomRange } ) );
	}
	void Molecule::remove( const AtomIndexRangeList & p_atomRangeList )
	{
		Util::Algorithm::Range::substractInSitu( _activeAtomIds, p_atomRangeList );

		for ( AtomIndexRangeList::RangeConstIterator it = p_atomRangeList.rangeBegin();
			  it != p_atomRangeList.rangeEnd();
			  it++ )
		{
			_deleteTopologyPointers( *it );
		}
		_resizeTopologyVectors();

		onAtomRemoved( p_atomRangeList );
	}

	void Molecule::_deleteTopologyPointers( const atom_index_t & p_atomIndex )
	{
		_atoms[ p_atomIndex ] = nullptr;

		const size_t residueIndex = _moleculeStruct.atomResidueIndexes[ p_atomIndex ];
		_refreshResidueRemovedState( residueIndex );

		const size_t chainIndex = _moleculeStruct.residueChainIndexes[ residueIndex ];
		_refreshChainRemovedState( chainIndex );
	}
	void Molecule::_deleteTopologyPointers( const AtomIndexRange & p_atomRange )
	{
		for ( atom_index_t iAtom = p_atomRange.getFirst(); iAtom <= p_atomRange.getLast(); iAtom++ )
			_atoms[ iAtom ] = nullptr;

		const size_t firstResidueIndex = _moleculeStruct.atomResidueIndexes[ p_atomRange.getFirst() ];
		const size_t lastResidueIndex  = _moleculeStruct.atomResidueIndexes[ p_atomRange.getLast() ];

		for ( size_t iResidue = firstResidueIndex + 1; iResidue <= lastResidueIndex - 1; iResidue++ )
			_residues[ iResidue ] = nullptr;

		_refreshResidueRemovedState( firstResidueIndex );
		_refreshResidueRemovedState( lastResidueIndex );

		const size_t firstChainIndex = _moleculeStruct.residueChainIndexes[ firstResidueIndex ];
		const size_t lastChainIndex	 = _moleculeStruct.atomResidueIndexes[ lastResidueIndex ];

		for ( size_t iChain = firstChainIndex + 1; iChain <= lastChainIndex - 1; iChain++ )
			_chains[ iChain ] = nullptr;

		_refreshChainRemovedState( firstChainIndex );
		_refreshChainRemovedState( lastChainIndex );
	}

	void Molecule::_refreshResidueRemovedState( const size_t p_residueIndex )
	{
		const Residue * const residue = getResidue( p_residueIndex );

		if ( residue != nullptr )
		{
			const AtomIndexRange atomRange = AtomIndexRange( residue->getIndexFirstAtom(), residue->getAtomCount() );

			if ( !_activeAtomIds.intersectWith( atomRange ) )
				_residues[ p_residueIndex ] = nullptr;
		}
	}
	void Molecule::_refreshChainRemovedState( const size_t p_chainIndex )
	{
		const Chain * const chain = getChain( p_chainIndex );

		if ( chain != nullptr )
		{
			const AtomIndexRange atomRange
				= AtomIndexRange::createFirstLast( chain->getIndexFirstAtom(), chain->getIndexLastAtom() );

			if ( !_activeAtomIds.intersectWith( atomRange ) )
				_chains[ p_chainIndex ] = nullptr;
		}
	}

	void Molecule::_resizeTopologyVectors()
	{
		eraseEndingNullValues( _chains );
		eraseEndingNullValues( _residues );
		eraseEndingNullValues( _atoms );
	}

} // namespace VTX::App::Component::Chemistry
