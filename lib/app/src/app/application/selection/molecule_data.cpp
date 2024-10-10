#include "app/application/selection/molecule_data.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/helper/math.hpp"
#include "app/vtx_app.hpp"
#include <sstream>
#include <util/algorithm/range.hpp>

namespace VTX::App::Application::Selection
{
	MoleculeData::MoleculeData( const Component::Scene::Selectable & p_selectable ) :
		SelectionData( p_selectable ), _molecule( &MAIN_REGISTRY().getComponent<Molecule>( p_selectable ) )
	{
		selectAll();
	}

	void MoleculeData::clear()
	{
		_chainIds.clear();
		_residueIds.clear();
		_atomIds.clear();

		_localAABB.invalidate();

		_currentObjectType = CurrentObjectTypeEnum::Molecule;
	}

	bool MoleculeData::isEqualsTo( const SelectionData & p_other ) const
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		return _molecule == castedOther._molecule && _chainIds == castedOther._chainIds
			   && _residueIds == castedOther._residueIds && _atomIds == castedOther._atomIds;
	}

	bool MoleculeData::isValid() const
	{
		return !( _chainIds.isEmpty() && _residueIds.isEmpty() && _atomIds.isEmpty() );
	}

	std::unique_ptr<SelectionData> MoleculeData::_cloneImpl() const
	{
		std::unique_ptr<MoleculeData> copy = std::make_unique<MoleculeData>( getSelectionComponent() );

		copy->_chainIds	  = _chainIds;
		copy->_residueIds = _residueIds;
		copy->_atomIds	  = _atomIds;

		copy->_currentObjectType  = _currentObjectType;
		copy->_currentObjectIndex = _currentObjectIndex;

		copy->_localAABB = _localAABB;

		return std::move( copy );
	}

	void MoleculeData::set( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		if ( _molecule == castedOther._molecule )
		{
			_chainIds	= castedOther._chainIds;
			_residueIds = castedOther._residueIds;
			_atomIds	= castedOther._atomIds;

			_localAABB = castedOther._localAABB;
		}
	}

	SelectionData & MoleculeData::add( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		if ( _molecule == castedOther._molecule )
		{
			Util::Algorithm::Range::mergeInSitu( _chainIds, castedOther._chainIds );
			Util::Algorithm::Range::mergeInSitu( _residueIds, castedOther._residueIds );
			Util::Algorithm::Range::mergeInSitu( _atomIds, castedOther._atomIds );

			_localAABB.extend( castedOther._localAABB );
		}

		return *this;
	}
	SelectionData & MoleculeData::remove( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		if ( _molecule == castedOther._molecule )
		{
			Util::Algorithm::Range::substractInSitu( _chainIds, castedOther._chainIds );
			Util::Algorithm::Range::substractInSitu( _residueIds, castedOther._residueIds );
			Util::Algorithm::Range::substractInSitu( _atomIds, castedOther._atomIds );

			_localAABB.invalidate();
		}

		return *this;
	}
	SelectionData & MoleculeData::intersect( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		if ( _molecule == castedOther._molecule )
		{
			Util::Algorithm::Range::intersectInSitu( _chainIds, castedOther._chainIds );
			Util::Algorithm::Range::intersectInSitu( _residueIds, castedOther._residueIds );
			Util::Algorithm::Range::intersectInSitu( _atomIds, castedOther._atomIds );
		}
		else
		{
			_chainIds.clear();
			_residueIds.clear();
			_atomIds.clear();
		}

		_localAABB.invalidate();

		return *this;
	}
	SelectionData & MoleculeData::exclude( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		if ( _molecule == castedOther._molecule )
		{
			_chainIds	= Util::Algorithm::Range::exclusive( _chainIds, castedOther._chainIds );
			_residueIds = Util::Algorithm::Range::exclusive( _residueIds, castedOther._residueIds );
			_atomIds	= Util::Algorithm::Range::exclusive( _atomIds, castedOther._atomIds );

			_localAABB.invalidate();
		}

		return *this;
	}
	bool MoleculeData::contains( const SelectionData & p_other ) const
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );
		return areAtomsSelected( castedOther._atomIds );
	}

	void MoleculeData::selectAll()
	{
		_chainIds.addRange( IndexRange( 0, _molecule->getChains().size() ) );
		_residueIds.addRange( IndexRange( 0, _molecule->getResidues().size() ) );
		_atomIds.addRange( AtomIndexRange( 0, atom_index_t( _molecule->getAtoms().size() ) ) );

		_localAABB.invalidate();

		setCurrentObject( *_molecule );
	}

	bool MoleculeData::isFullySelected() const { return _atomIds.count() == _molecule->getAtoms().size(); }

	// Chains ////////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::referenceChain( const Chain & p_chain )
	{
		_referenceChain( p_chain );
		setCurrentObject( p_chain );
	}
	void MoleculeData::selectFullChain( const Chain & p_chain )
	{
		_selectFullChain( p_chain );
		setCurrentObject( p_chain );
	}
	void MoleculeData::referenceChains( const IndexRange & p_range )
	{
		_referenceChains( p_range );
		setCurrentObject( *_molecule->getChain( p_range.getLast() ) );
	}
	void MoleculeData::selectFullChains( const IndexRange & p_range )
	{
		_selectFullChains( p_range );
		setCurrentObject( *_molecule->getChain( p_range.getLast() ) );
	}
	void MoleculeData::referenceChains( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Chain * const chainPtr = _molecule->getChain( index );

			if ( chainPtr != nullptr )
				_referenceChain( *chainPtr );
		}

		setCurrentObject( *_molecule->getChain( p_rangeList.getLast() ) );
	}
	void MoleculeData::selectFullChains( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Chain * const chainPtr = _molecule->getChain( index );

			if ( chainPtr != nullptr )
				_selectFullChain( *chainPtr );
		}

		setCurrentObject( *_molecule->getChain( p_rangeList.getLast() ) );
	}

	void MoleculeData::unselectChain( const Chain & p_chain )
	{
		_unselectChain( p_chain );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectChain( const size_t p_chainIndex )
	{
		unselectChain( *_molecule->getChain( p_chainIndex ) );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectChains( const IndexRange & p_chains )
	{
		_unselectChains( p_chains );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectChains( const IndexRangeList & p_chain )
	{
		for ( auto it = p_chain.rangeBegin(); it != p_chain.rangeEnd(); ++it )
		{
			_unselectChains( *it );
		}
		_refreshCurrentObject();
	}
	void MoleculeData::unselectChains( const std::initializer_list<const Chain *> & p_chains )
	{
		for ( const Chain * const chainPtr : p_chains )
		{
			if ( chainPtr != nullptr )
				_unselectChain( *chainPtr );
		}
		_refreshCurrentObject();
	}
	void MoleculeData::unselectChains( const std::initializer_list<size_t> & p_chains )
	{
		for ( const size_t chainIndex : p_chains )
		{
			_unselectChain( *_molecule->getChain( chainIndex ) );
		}

		_refreshCurrentObject();
	}

	bool MoleculeData::isChainSelected( const size_t & p_chainIndex ) const
	{
		return _chainIds.contains( p_chainIndex );
	}
	bool MoleculeData::isChainSelected( const Chain & p_chain ) const { return isChainSelected( p_chain.getIndex() ); }
	bool MoleculeData::isChainFullySelected( const Chain & p_chain ) const
	{
		return areAtomsSelected(
			AtomIndexRange::createFirstLast( p_chain.getIndexFirstAtom(), p_chain.getIndexLastAtom() )
		);
	}
	bool MoleculeData::isChainFullySelected( const size_t & p_chainIndex ) const
	{
		return isChainFullySelected( *_molecule->getChain( p_chainIndex ) );
	}
	bool MoleculeData::areChainsSelected( const IndexRange & p_chains ) const { return _chainIds.contains( p_chains ); }
	bool MoleculeData::areChainsSelected( const IndexRangeList & p_chains ) const
	{
		return _chainIds.contains( p_chains );
	}
	bool MoleculeData::areChainsSelected( const std::initializer_list<const Chain *> & p_chains ) const
	{
		if ( p_chains.size() == 0 )
			return false;

		for ( const Chain * const chain : p_chains )
			if ( !isChainSelected( chain->getIndex() ) )
				return false;

		return true;
	}
	bool MoleculeData::areChainsSelected( const std::initializer_list<size_t> & p_chains ) const
	{
		if ( p_chains.size() == 0 )
			return false;

		for ( const size_t chainIndex : p_chains )
			if ( !isChainSelected( chainIndex ) )
				return false;

		return true;
	}
	bool MoleculeData::areChainsFullySelected( const IndexRange & p_chains ) const
	{
		const atom_index_t firstAtom = _molecule->getChain( p_chains.getFirst() )->getIndexFirstAtom();
		const atom_index_t lastAtom	 = _molecule->getChain( p_chains.getLast() )->getIndexLastAtom();

		return _atomIds.contains( AtomIndexRange::createFirstLast( firstAtom, lastAtom ) );
	}
	bool MoleculeData::areChainsFullySelected( const IndexRangeList & p_chains ) const
	{
		for ( Util::Math::RangeList<size_t>::RangeConstIterator itRange = p_chains.rangeBegin();
			  itRange != p_chains.rangeEnd();
			  itRange++ )
		{
			if ( !areResiduesFullySelected( *itRange ) )
				return false;
		}

		return true;
	}
	bool MoleculeData::areChainsFullySelected( const std::initializer_list<const Chain *> & p_chains ) const
	{
		for ( const Chain * const chainPtr : p_chains )
			if ( !isChainSelected( *chainPtr ) )
				return false;

		return true;
	}
	bool MoleculeData::areChainsFullySelected( const std::initializer_list<size_t> & p_chains ) const
	{
		for ( const size_t & chainIndex : p_chains )
			if ( !isChainSelected( chainIndex ) )
				return false;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Residues //////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::referenceResidue( const Residue & p_residue )
	{
		_referenceChain( *p_residue.getConstChainPtr() );
		_referenceResidue( p_residue );

		setCurrentObject( p_residue );
	}
	void MoleculeData::selectFullResidue( const Residue & p_residue )
	{
		_referenceChain( *p_residue.getConstChainPtr() );
		_selectFullResidue( p_residue );

		setCurrentObject( p_residue );
	}
	void MoleculeData::referenceResidues( const IndexRange & p_range )
	{
		const size_t firstChainIndex = _molecule->getResidue( p_range.getFirst() )->getConstChainPtr()->getIndex();
		const size_t lastChainIndex	 = _molecule->getResidue( p_range.getLast() )->getConstChainPtr()->getIndex();

		_referenceChains( IndexRange::createFirstLast( firstChainIndex, lastChainIndex ) );
		_referenceResidues( p_range );

		setCurrentObject( *( _molecule->getResidue( p_range.getLast() ) ) );
	}
	void MoleculeData::selectFullResidues( const IndexRange & p_range )
	{
		_selectFullResidues( p_range );
		setCurrentObject( *( _molecule->getResidue( p_range.getLast() ) ) );
	}
	void MoleculeData::referenceResidues( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Residue * const residuePtr = _molecule->getResidue( index );
			_referenceChain( *residuePtr->getConstChainPtr() );

			if ( residuePtr != nullptr )
				_referenceResidue( *residuePtr );
		}

		setCurrentObject( *( _molecule->getResidue( p_rangeList.getLast() ) ) );
	}
	void MoleculeData::selectFullResidues( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Residue * const residuePtr = _molecule->getResidue( index );
			_referenceChain( *residuePtr->getConstChainPtr() );

			if ( residuePtr != nullptr )
				_selectFullResidue( *residuePtr );
		}

		setCurrentObject( *( _molecule->getResidue( p_rangeList.getLast() ) ) );
	}

	void MoleculeData::unselectResidue( const Residue & p_residue )
	{
		_unselectResidue( p_residue );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectResidue( const size_t p_residueIndex )
	{
		_unselectResidue( *_molecule->getResidue( p_residueIndex ) );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectResidues( const IndexRange & p_residues )
	{
		_unselectResidues( p_residues );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectResidues( const IndexRangeList & p_residues )
	{
		for ( Util::Math::RangeList<size_t>::RangeConstIterator itRange = p_residues.rangeBegin();
			  itRange != p_residues.rangeEnd();
			  ++itRange )
		{
			_unselectResidues( *itRange );
		}

		_refreshCurrentObject();
	}
	void MoleculeData::unselectResidues( const std::initializer_list<const Residue *> & p_atoms )
	{
		for ( const Residue * const residuePtr : p_atoms )
		{
			if ( residuePtr != nullptr )
				_unselectResidue( *residuePtr );
		}

		_refreshCurrentObject();
	}
	void MoleculeData::unselectResidues( const std::initializer_list<size_t> & p_atoms )
	{
		for ( const size_t atomIndex : p_atoms )
		{
			const Residue * const atomPtr = _molecule->getResidue( atomIndex );

			if ( atomPtr != nullptr )
				_unselectResidue( *atomPtr );
		}

		_refreshCurrentObject();
	}

	bool MoleculeData::isResidueSelected( const size_t & p_residueIndex ) const
	{
		return _residueIds.contains( p_residueIndex );
	}
	bool MoleculeData::isResidueSelected( const Residue & p_residue ) const
	{
		return _residueIds.contains( p_residue.getIndex() );
	}
	bool MoleculeData::isResidueFullySelected( const size_t & p_residueIndex ) const
	{
		return isResidueFullySelected( *_molecule->getResidue( p_residueIndex ) );
	}
	bool MoleculeData::isResidueFullySelected( const Residue & p_residue ) const
	{
		return _atomIds.contains( AtomIndexRange( p_residue.getIndexFirstAtom(), p_residue.getAtomCount() ) );
	}
	bool MoleculeData::areResiduesSelected( const IndexRange & p_residues ) const
	{
		return _residueIds.contains( p_residues );
	}
	bool MoleculeData::areResiduesSelected( const IndexRangeList & p_residues ) const
	{
		return _residueIds.contains( p_residues );
	}
	bool MoleculeData::areResiduesSelected( const std::initializer_list<const Residue *> & p_residues ) const
	{
		for ( const Residue * const residuePtr : p_residues )
			if ( !isResidueSelected( *residuePtr ) )
				return false;

		return true;
	}
	bool MoleculeData::areResiduesSelected( const std::initializer_list<size_t> & p_residues ) const
	{
		for ( const size_t & residueIndex : p_residues )
			if ( !isResidueSelected( residueIndex ) )
				return false;

		return true;
	}
	bool MoleculeData::areResiduesFullySelected( const IndexRange & p_residues ) const
	{
		const atom_index_t firstAtom = _molecule->getResidue( p_residues.getFirst() )->getIndexFirstAtom();
		const atom_index_t lastAtom	 = _molecule->getResidue( p_residues.getLast() )->getIndexLastAtom();

		return _atomIds.contains( AtomIndexRange::createFirstLast( firstAtom, lastAtom ) );
	}
	bool MoleculeData::areResiduesFullySelected( const IndexRangeList & p_residues ) const
	{
		for ( Util::Math::RangeList<size_t>::RangeConstIterator itRange = p_residues.rangeBegin();
			  itRange != p_residues.rangeEnd();
			  itRange++ )
		{
			if ( !areResiduesFullySelected( *itRange ) )
				return false;
		}

		return true;
	}
	bool MoleculeData::areResiduesFullySelected( const std::initializer_list<const Residue *> & p_residues ) const
	{
		for ( const Residue * const residue : p_residues )
		{
			if ( !isResidueFullySelected( *residue ) )
				return false;
		}

		return true;
	}
	bool MoleculeData::areResiduesFullySelected( const std::initializer_list<size_t> & p_residues ) const
	{
		for ( const size_t residueIndex : p_residues )
		{
			if ( !isResidueFullySelected( residueIndex ) )
				return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Atoms /////////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::selectAtom( const Atom & p_atom )
	{
		_referenceResidue( *p_atom.getConstResiduePtr() );
		_referenceChain( *p_atom.getConstChainPtr() );

		_referenceAtom( p_atom );

		setCurrentObject( p_atom );
	}
	void MoleculeData::selectAtoms( const AtomIndexRange & p_range )
	{
		const Residue & firstResidue = *_molecule->getAtom( p_range.getFirst() )->getConstResiduePtr();
		const Residue & lastResidue	 = *_molecule->getAtom( p_range.getLast() )->getConstResiduePtr();

		_referenceResidues( IndexRange::createFirstLast( firstResidue.getIndex(), lastResidue.getIndex() ) );

		_referenceChains( IndexRange::createFirstLast(
			firstResidue.getConstChainPtr()->getIndex(), lastResidue.getConstChainPtr()->getIndex()
		) );

		_referenceAtoms( p_range );

		setCurrentObject( *_molecule->getAtom( p_range.getLast() ) );
	}
	void MoleculeData::selectAtoms( const AtomIndexRangeList & p_rangeList )
	{
		for ( AtomIndexRangeList::RangeConstIterator it = p_rangeList.rangeBegin(); it != p_rangeList.rangeEnd(); it++ )
		{
			const Residue & firstResidue = *_molecule->getAtom( it->getFirst() )->getConstResiduePtr();
			const Residue & lastResidue	 = *_molecule->getAtom( it->getLast() )->getConstResiduePtr();

			_referenceResidues( IndexRange::createFirstLast( firstResidue.getIndex(), lastResidue.getIndex() ) );

			const size_t firstChainIndex = firstResidue.getConstChainPtr()->getIndex();
			const size_t lastChainIndex	 = lastResidue.getConstChainPtr()->getIndex();
			_referenceChains( IndexRange::createFirstLast( firstChainIndex, lastChainIndex ) );

			_referenceAtoms( *it );
		}

		setCurrentObject( *_molecule->getAtom( p_rangeList.getLast() ) );
	}

	void MoleculeData::unselectAtom( const Atom & p_atom )
	{
		_unselectAtom( p_atom );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectAtom( const atom_index_t p_atomIndex )
	{
		_unselectAtom( *_molecule->getAtom( p_atomIndex ) );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectAtoms( const AtomIndexRange & p_atoms )
	{
		_unselectAtoms( p_atoms );
		_refreshCurrentObject();
	}
	void MoleculeData::unselectAtoms( const AtomIndexRangeList & p_atoms )
	{
		for ( AtomIndexRangeList::RangeConstIterator itRange = p_atoms.rangeBegin(); itRange != p_atoms.rangeEnd();
			  ++itRange )
		{
			unselectAtoms( *itRange );
		}

		_refreshCurrentObject();
	}
	void MoleculeData::unselectAtoms( const std::initializer_list<const Atom *> & p_atoms )
	{
		for ( const Atom * const atomPtr : p_atoms )
		{
			if ( atomPtr != nullptr )
				_unselectAtom( *atomPtr );
		}

		_refreshCurrentObject();
	}
	void MoleculeData::unselectAtoms( const std::initializer_list<atom_index_t> & p_atoms )
	{
		for ( const atom_index_t atomIndex : p_atoms )
		{
			const Atom * const atomPtr = _molecule->getAtom( atomIndex );

			if ( atomPtr != nullptr )
				_unselectAtom( *atomPtr );
		}

		_refreshCurrentObject();
	}

	bool MoleculeData::isAtomSelected( const atom_index_t & p_atomIndex ) const
	{
		return _atomIds.contains( p_atomIndex );
	}
	bool MoleculeData::isAtomSelected( const Atom & p_atom ) const { return _atomIds.contains( p_atom.getIndex() ); }
	bool MoleculeData::areAtomsSelected( const AtomIndexRange & p_atoms ) const { return _atomIds.contains( p_atoms ); }
	bool MoleculeData::areAtomsSelected( const AtomIndexRangeList & p_atoms ) const
	{
		return _atomIds.contains( p_atoms );
	}
	bool MoleculeData::areAtomsSelected( const std::initializer_list<const Atom *> & p_atoms ) const
	{
		for ( const Atom * const atom : p_atoms )
		{
			if ( !isAtomSelected( *atom ) )
				return false;
		}
		return true;
	}
	bool MoleculeData::areAtomsSelected( const std::initializer_list<atom_index_t> & p_atoms ) const
	{
		for ( atom_index_t const atomIndex : p_atoms )
		{
			if ( !isAtomSelected( atomIndex ) )
				return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void MoleculeData::_referenceChain( const Chain & p_chain ) { _chainIds.addValue( p_chain.getIndex() ); }
	void MoleculeData::_selectFullChain( const Chain & p_chain )
	{
		const size_t firstResidueIndex = p_chain.getIndexFirstResidue();
		const size_t residueCount	   = p_chain.getResidueCount();

		_selectFullResidues( IndexRange( firstResidueIndex, residueCount ) );
		_referenceChain( p_chain );
	}
	void MoleculeData::_referenceChains( const IndexRange & p_range ) { _chainIds.addRange( p_range ); }
	void MoleculeData::_selectFullChains( const IndexRange & p_range )
	{
		const Chain & firstChain = *_molecule->getChain( p_range.getFirst() );
		const Chain & lastChain	 = *_molecule->getChain( p_range.getLast() );

		const size_t firstResidueIndex = firstChain.getIndexFirstResidue();
		const size_t lastResidueIndex  = lastChain.getIndexFirstResidue() + lastChain.getResidueCount() - 1;

		_selectFullResidues( IndexRange::createFirstLast( firstResidueIndex, lastResidueIndex ) );
		_referenceChains( p_range );
	}
	void MoleculeData::_unselectChain( const Chain & p_chain )
	{
		_chainIds.removeValue( p_chain.getIndex() );
		_residueIds.removeRange(
			IndexRange::createFirstLast( p_chain.getIndexFirstResidue(), p_chain.getIndexLastResidue() )
		);
		_atomIds.removeRange( AtomIndexRange::createFirstLast( p_chain.getIndexFirstAtom(), p_chain.getIndexLastAtom() )
		);
	}
	void MoleculeData::_unselectChains( const IndexRange & p_range )
	{
		const Chain & firstChain = *_molecule->getChain( p_range.getFirst() );
		const Chain & lastChain	 = *_molecule->getChain( p_range.getLast() );

		_atomIds.removeRange(
			AtomIndexRange::createFirstLast( firstChain.getIndexFirstAtom(), lastChain.getIndexLastAtom() )
		);
		_residueIds.removeRange(
			IndexRange::createFirstLast( firstChain.getIndexFirstResidue(), lastChain.getIndexLastResidue() )
		);
		_chainIds.removeRange( p_range );
	}

	void MoleculeData::_referenceResidue( const Residue & p_residue ) { _residueIds.addValue( p_residue.getIndex() ); }
	void MoleculeData::_selectFullResidue( const Residue & p_residue )
	{
		const atom_index_t firstAtomIndex = p_residue.getIndexFirstAtom();
		const atom_index_t atomCount	  = p_residue.getAtomCount();

		_referenceAtoms( AtomIndexRange( firstAtomIndex, atomCount ) );
		_referenceResidue( p_residue );
	}
	void MoleculeData::_referenceResidues( const IndexRange & p_range ) { _residueIds.addRange( p_range ); }
	void MoleculeData::_selectFullResidues( const IndexRange & p_range )
	{
		const Residue * const firstResidue = _molecule->getResidue( p_range.getFirst() );
		const Residue * const lastResidue  = _molecule->getResidue( p_range.getLast() );

		const atom_index_t firstAtomIndex = firstResidue->getIndexFirstAtom();
		const atom_index_t lastAtomIndex  = lastResidue->getIndexFirstAtom() + lastResidue->getAtomCount() - 1;

		_referenceAtoms( AtomIndexRange::createFirstLast( firstAtomIndex, lastAtomIndex ) );
		_referenceResidues( p_range );
	}
	void MoleculeData::_unselectResidue( const Residue & p_residue )
	{
		_residueIds.removeValue( p_residue.getIndex() );
		_atomIds.removeRange(
			AtomIndexRange::createFirstLast( p_residue.getIndexFirstAtom(), p_residue.getIndexLastAtom() )
		);

		const Chain & chainParent = *p_residue.getConstChainPtr();
		if ( !_residueIds.intersectWith(
				 IndexRange::createFirstLast( chainParent.getIndexFirstResidue(), chainParent.getIndexLastResidue() )
			 ) )
		{
			_unselectChain( chainParent );
		}
	}
	void MoleculeData::_unselectResidues( const IndexRange & p_range )
	{
		const Residue & firstResidue = *_molecule->getResidue( p_range.getFirst() );
		const Residue & lastResidue	 = *_molecule->getResidue( p_range.getLast() );

		// Atoms
		const atom_index_t firstAtomIndex = firstResidue.getIndexFirstAtom();
		const atom_index_t lastAtomIndex  = lastResidue.getIndexLastAtom();
		_atomIds.removeRange( AtomIndexRange::createFirstLast( firstAtomIndex, lastAtomIndex ) );

		// Residues
		_residueIds.removeRange( p_range );

		// Chains
		const Chain & firstChainParent = *firstResidue.getConstChainPtr();
		const size_t  firstChainIndex  = firstResidue.getIndex();
		const Chain & lastChainParent  = *lastResidue.getConstChainPtr();
		const size_t  lastChainIndex   = lastResidue.getIndex();

		if ( !_residueIds.intersectWith( IndexRange::createFirstLast(
				 firstChainParent.getIndexFirstResidue(), firstChainParent.getIndexLastResidue()
			 ) ) )
		{
			_chainIds.removeValue( firstChainParent.getIndex() );
		}
		if ( lastChainIndex - firstChainIndex >= 2 )
		{
			_chainIds.removeRange( IndexRange::createFirstLast( firstChainIndex + 1, lastChainIndex - 1 ) );
		}
		if ( !_residueIds.intersectWith( IndexRange::createFirstLast(
				 lastChainParent.getIndexFirstResidue(), lastChainParent.getIndexLastResidue()
			 ) ) )
		{
			_chainIds.removeValue( lastChainParent.getIndex() );
		}
	}

	void MoleculeData::_referenceAtom( const Atom & p_atom )
	{
		_atomIds.addValue( p_atom.getIndex() );
		_localAABB.extend( p_atom.getLocalPosition(), p_atom.getVdwRadius() );
	}
	void MoleculeData::_referenceAtoms( const AtomIndexRange & p_range )
	{
		_atomIds.addRange( p_range );

		for ( atom_index_t atomIndex = p_range.getFirst(); atomIndex <= p_range.getLast(); atomIndex++ )
		{
			const Atom & atom = *_molecule->getAtom( atomIndex );
			_localAABB.extend( atom.getLocalPosition(), atom.getVdwRadius() );
		}
	}
	void MoleculeData::_unselectAtom( const Atom & p_atom )
	{
		const Residue & residueParent = *p_atom.getConstResiduePtr();
		const Chain &	chainParent	  = *residueParent.getConstChainPtr();

		_atomIds.removeValue( p_atom.getIndex() );

		if ( !_atomIds.intersectWith(
				 AtomIndexRange::createFirstLast( residueParent.getIndexFirstAtom(), residueParent.getIndexLastAtom() )
			 ) )
		{
			_unselectResidue( residueParent );
		}

		if ( !_atomIds.intersectWith(
				 AtomIndexRange::createFirstLast( residueParent.getIndexFirstAtom(), residueParent.getIndexLastAtom() )
			 ) )
		{
			_unselectChain( chainParent );
		}

		_localAABB.invalidate();
	}
	void MoleculeData::_unselectAtoms( const AtomIndexRange & p_range )
	{
		// Atoms
		_atomIds.removeRange( p_range );

		// Residues
		const Residue & firstResidueParent = *_molecule->getAtom( p_range.getFirst() )->getConstResiduePtr();
		const size_t	firstResidueIndex  = firstResidueParent.getIndex();
		const Residue & lastResidueParent  = *_molecule->getAtom( p_range.getLast() )->getConstResiduePtr();
		const size_t	lastResidueIndex   = lastResidueParent.getIndex();

		if ( !_atomIds.intersectWith( AtomIndexRange::createFirstLast(
				 firstResidueParent.getIndexFirstAtom(), firstResidueParent.getIndexLastAtom()
			 ) ) )
		{
			_residueIds.removeValue( firstResidueParent.getIndex() );
		}
		if ( lastResidueIndex - firstResidueIndex >= 2 )
		{
			_residueIds.removeRange( IndexRange::createFirstLast( firstResidueIndex + 1, lastResidueIndex - 1 ) );
		}
		if ( !_atomIds.intersectWith( AtomIndexRange::createFirstLast(
				 lastResidueParent.getIndexFirstAtom(), lastResidueParent.getIndexLastAtom()
			 ) ) )
		{
			_residueIds.removeValue( lastResidueParent.getIndex() );
		}

		// Chains
		const Chain & firstChainParent = *firstResidueParent.getConstChainPtr();
		const size_t  firstChainIndex  = firstResidueParent.getIndex();
		const Chain & lastChainParent  = *lastResidueParent.getConstChainPtr();
		const size_t  lastChainIndex   = lastResidueParent.getIndex();
		if ( !_atomIds.intersectWith( AtomIndexRange::createFirstLast(
				 firstChainParent.getIndexFirstAtom(), firstChainParent.getIndexLastAtom()
			 ) ) )
		{
			_chainIds.removeValue( firstChainParent.getIndex() );
		}
		if ( lastChainIndex - firstChainIndex >= 2 )
		{
			_chainIds.removeRange( IndexRange::createFirstLast( firstChainIndex + 1, lastChainIndex - 1 ) );
		}
		if ( !_atomIds.intersectWith( AtomIndexRange::createFirstLast(
				 lastChainParent.getIndexFirstAtom(), lastChainParent.getIndexLastAtom()
			 ) ) )
		{
			_chainIds.removeValue( lastChainParent.getIndex() );
		}

		_localAABB.invalidate();
	}

	Util::Math::AABB MoleculeData::getAABB() const
	{
		if ( !_localAABB.isValid() )
			_recomputeAABB();

		const Util::Math::Transform & transform
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *_molecule ).getTransform();

		return Helper::Math::applyTransformOnAABB( _localAABB, transform );
	}

	void MoleculeData::_recomputeAABB() const
	{
		_localAABB.invalidate();

		for ( const atom_index_t atomId : _atomIds )
		{
			const Component::Chemistry::Atom * const atomPtr = _molecule->getAtom( atomId );
			_localAABB.extend( atomPtr->getLocalPosition(), atomPtr->getVdwRadius() );
		}
	}

	Molecule & MoleculeData::getCurrentObjectAsMolecule() const
	{
		assert( _currentObjectType == CurrentObjectTypeEnum::Molecule );
		return *_molecule;
	}
	Chain & MoleculeData::getCurrentObjectAsChain() const
	{
		assert( _currentObjectType == CurrentObjectTypeEnum::Chain );
		return *_molecule->getChain( _currentObjectIndex );
	}
	Residue & MoleculeData::getCurrentObjectAsResidue() const
	{
		assert( _currentObjectType == CurrentObjectTypeEnum::Residue );
		return *_molecule->getResidue( _currentObjectIndex );
	}
	Atom & MoleculeData::getCurrentObjectAsAtom() const
	{
		assert( _currentObjectType == CurrentObjectTypeEnum::Atom );
		return *_molecule->getAtom( atom_index_t( _currentObjectIndex ) );
	}

	void MoleculeData::setCurrentObject( const Molecule & p_molecule )
	{
		_currentObjectType = CurrentObjectTypeEnum::Molecule;
	}
	void MoleculeData::setCurrentObject( const Chain & p_chain )
	{
		_currentObjectIndex = p_chain.getIndex();
		_currentObjectType	= CurrentObjectTypeEnum::Chain;
	}
	void MoleculeData::setCurrentObject( const Residue & p_residue )
	{
		_currentObjectIndex = p_residue.getIndex();
		_currentObjectType	= CurrentObjectTypeEnum::Residue;
	}
	void MoleculeData::setCurrentObject( const Atom & p_atom )
	{
		_currentObjectIndex = p_atom.getIndex();
		_currentObjectType	= CurrentObjectTypeEnum::Atom;
	}

	void MoleculeData::_refreshCurrentObject()
	{
		bool currentObjectAsChange = false;

		switch ( _currentObjectType )
		{
		case CurrentObjectTypeEnum::Molecule: break;
		case CurrentObjectTypeEnum::Chain: currentObjectAsChange = !isChainSelected( _currentObjectIndex ); break;
		case CurrentObjectTypeEnum::Residue: currentObjectAsChange = !isResidueSelected( _currentObjectIndex ); break;
		case CurrentObjectTypeEnum::Atom:
			currentObjectAsChange = !isAtomSelected( atom_index_t( _currentObjectIndex ) );
			break;
		}

		if ( currentObjectAsChange )
		{
			_currentObjectIndex = INVALID_INDEX;
			_currentObjectType	= CurrentObjectTypeEnum::None;
		}
	}

	std::string MoleculeData::toString() const
	{
		std::stringstream sStr			   = std::stringstream();
		const int		  maxItemDisplayed = 20;

		sStr << SelectionData::toString() << std::endl;
		sStr << "Chains  (" << _chainIds.count() << '/' << _molecule->getChains().size() << ")" << std::endl;

		int counter = 0;

		for ( const size_t chainId : _chainIds )
		{
			const Chain & chain = *_molecule->getChain( chainId );
			sStr << chain.getName() << '(' << chainId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl << std::endl;
		counter = 0;

		sStr << "Residues  (" << _residueIds.count() << '/' << _molecule->getResidues().size() << ")" << std::endl;
		for ( const size_t residueId : _residueIds )
		{
			const Residue & residue = *_molecule->getResidue( residueId );
			sStr << residue.getName() << '(' << residueId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl << std::endl;
		counter = 0;

		sStr << "Atoms  (" << _atomIds.count() << '/' << _molecule->getAtoms().size() << ")" << std::endl;
		for ( const atom_index_t atomId : _atomIds )
		{
			const Atom & atom = *_molecule->getAtom( atomId );
			sStr << atom.getName() << '(' << atomId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl;

		return sStr.str();
	}
} // namespace VTX::App::Application::Selection
