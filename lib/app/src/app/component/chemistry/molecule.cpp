#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/uid/uid_system.hpp"
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
		p_vector.shrink_to_fit();
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
	Molecule::Molecule( VTX::Core::Struct::System & p_moleculeStruct ) { setMoleculeStruct( p_moleculeStruct ); }
	Molecule::~Molecule()
	{
		if ( _atomUidRange.isValid() )
			UID_SYSTEM().unregister( _atomUidRange );

		if ( _residueUidRange.isValid() )
			UID_SYSTEM().unregister( _residueUidRange );
	};

	void Molecule::setMoleculeStruct( VTX::Core::Struct::System & p_moleculeStruct )
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

		_realChainCount = p_chainCount;
	}

	void Molecule::initResidues( const size_t p_residueCount )
	{
		_residues.resize( p_residueCount );
		std::generate(
			_residues.begin(),
			_residues.end(),
			[ this, n = 0 ]() mutable { return std::move( std::make_unique<Residue>( this, n++ ) ); }
		);

		_realResidueCount = p_residueCount;
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
		_visibleAtomIds.addRange( AtomIndexRange( 0, atom_index_t( p_atomCount ) ) );
		_activeAtomIds.addRange( AtomIndexRange( 0, atom_index_t( p_atomCount ) ) );

		_realAtomCount = p_atomCount;
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
			= ECS_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *this );
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

		onAtomRemoved( AtomIndexRangeList( p_atomIndex ) );
	}
	void Molecule::remove( const AtomIndexRange & p_atomRange )
	{
		_activeAtomIds.removeRange( p_atomRange );
		_deleteTopologyPointers( p_atomRange );
		_resizeTopologyVectors();

		onAtomRemoved( AtomIndexRangeList( { p_atomRange } ) );
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

	void Molecule::_deleteTopologyPointers( const atom_index_t p_atomIndex )
	{
		_internalDeleteAtom( p_atomIndex );

		const size_t residueIndex = _moleculeStruct.atomResidueIndexes[ p_atomIndex ];
		_refreshResidueRemovedState( residueIndex );

		const size_t chainIndex = _moleculeStruct.residueChainIndexes[ residueIndex ];
		_refreshChainRemovedState( chainIndex );
	}
	void Molecule::_deleteTopologyPointers( const AtomIndexRange & p_atomRange )
	{
		_internalDeleteAtoms( p_atomRange );

		const size_t firstResidueIndex = _moleculeStruct.atomResidueIndexes[ p_atomRange.getFirst() ];
		const size_t lastResidueIndex  = _moleculeStruct.atomResidueIndexes[ p_atomRange.getLast() ];

		if ( firstResidueIndex == lastResidueIndex )
		{
			_refreshResidueRemovedState( firstResidueIndex );
		}
		else
		{
			const ResidueIndexRange rangeToDelete
				= ResidueIndexRange::createFirstLast( firstResidueIndex + 1, lastResidueIndex - 1 );

			if ( rangeToDelete.isValid() )
				_internalDeleteResidues( rangeToDelete );

			_refreshResidueRemovedState( firstResidueIndex );
			_refreshResidueRemovedState( lastResidueIndex );
		}

		const size_t firstChainIndex = _moleculeStruct.residueChainIndexes[ firstResidueIndex ];
		const size_t lastChainIndex	 = _moleculeStruct.residueChainIndexes[ lastResidueIndex ];

		if ( firstChainIndex == lastChainIndex )
		{
			_refreshChainRemovedState( firstChainIndex );
		}
		else
		{
			const ChainIndexRange rangeToDelete
				= ChainIndexRange::createFirstLast( firstChainIndex + 1, lastChainIndex - 1 );

			if ( rangeToDelete.isValid() )
				_internalDeleteChains( rangeToDelete );

			_refreshChainRemovedState( firstChainIndex );
			_refreshChainRemovedState( lastChainIndex );
		}
	}

	void Molecule::_refreshResidueRemovedState( const size_t p_residueIndex )
	{
		const Residue * const residue = getResidue( p_residueIndex );

		if ( residue != nullptr )
		{
			const AtomIndexRange atomRange = residue->getAtomRange();

			if ( !_activeAtomIds.intersectWith( atomRange ) )
			{
				_internalDeleteResidue( p_residueIndex );
			}
			else
			{
				// Refresh first atom and count
				while ( _atoms[ _moleculeStruct.residueFirstAtomIndexes[ p_residueIndex ] ] == nullptr )
				{
					_moleculeStruct.residueFirstAtomIndexes[ p_residueIndex ]++;
					_moleculeStruct.residueAtomCounts[ p_residueIndex ]--;
				}

				atom_index_t lastAtom = _moleculeStruct.residueFirstAtomIndexes[ p_residueIndex ]
										+ _moleculeStruct.residueAtomCounts[ p_residueIndex ] - 1;

				while ( _atoms[ lastAtom ] == nullptr )
				{
					_moleculeStruct.residueAtomCounts[ p_residueIndex ]--;
					lastAtom--;
				}
			}
		}
	}
	void Molecule::_refreshChainRemovedState( const size_t p_chainIndex )
	{
		const Chain * const chain = getChain( p_chainIndex );

		if ( chain != nullptr )
		{
			const atom_index_t firstAtomIndex
				= _moleculeStruct.residueFirstAtomIndexes[ _moleculeStruct.chainFirstResidues[ p_chainIndex ] ];

			const size_t lastResidueIndex = _moleculeStruct.chainFirstResidues[ p_chainIndex ]
											+ _moleculeStruct.chainResidueCounts[ p_chainIndex ] - 1;

			const atom_index_t lastAtomIndex = _moleculeStruct.residueFirstAtomIndexes[ lastResidueIndex ]
											   + _moleculeStruct.residueAtomCounts[ lastResidueIndex ] - 1;

			const AtomIndexRange atomRange = AtomIndexRange::createFirstLast( firstAtomIndex, lastAtomIndex );

			if ( !_activeAtomIds.intersectWith( atomRange ) )
			{
				_internalDeleteChain( p_chainIndex );
			}
			else
			{
				while ( _residues[ _moleculeStruct.chainFirstResidues[ p_chainIndex ] ] == nullptr )
				{
					_moleculeStruct.chainFirstResidues[ p_chainIndex ]++;
					_moleculeStruct.chainResidueCounts[ p_chainIndex ]--;
				}

				size_t lastResidue = _moleculeStruct.chainFirstResidues[ p_chainIndex ]
									 + _moleculeStruct.chainResidueCounts[ p_chainIndex ] - 1;

				while ( _residues[ lastResidue ] == nullptr )
				{
					_moleculeStruct.chainResidueCounts[ p_chainIndex ]--;
					lastResidue--;
				}
			}
		}
	}

	void Molecule::_internalDeleteAtom( const atom_index_t p_index )
	{
		_atoms[ p_index ] = nullptr;
		_realAtomCount--;
	}
	void Molecule::_internalDeleteAtoms( const AtomIndexRange & p_range )
	{
		for ( atom_index_t iAtom = p_range.getFirst(); iAtom <= p_range.getLast(); iAtom++ )
			_atoms[ iAtom ] = nullptr;

		_realAtomCount -= p_range.getCount();
	}
	void Molecule::_internalDeleteResidue( const size_t p_index )
	{
		_residues[ p_index ] = nullptr;
		_realResidueCount--;
	}
	void Molecule::_internalDeleteResidues( const ResidueIndexRange & p_range )
	{
		for ( size_t iResidue = p_range.getFirst(); iResidue <= p_range.getLast(); iResidue++ )
			_residues[ iResidue ] = nullptr;

		_realResidueCount -= p_range.getCount();
	}
	void Molecule::_internalDeleteChain( const size_t p_index )
	{
		_chains[ p_index ] = nullptr;
		_realChainCount--;
	}
	void Molecule::_internalDeleteChains( const ChainIndexRange & p_range )
	{
		for ( size_t iChain = p_range.getFirst(); iChain <= p_range.getLast(); iChain++ )
			_chains[ iChain ] = nullptr;

		_realChainCount -= p_range.getCount();
	}

	void Molecule::_resizeTopologyVectors()
	{
		eraseEndingNullValues( _chains );
		eraseEndingNullValues( _residues );
		eraseEndingNullValues( _atoms );
	}

} // namespace VTX::App::Component::Chemistry
