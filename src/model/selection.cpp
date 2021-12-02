#include "selection.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "define.hpp"
#include "event/event_manager.hpp"
#include "molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "residue.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include <unordered_set>

namespace VTX::Model
{
	bool Selection::hasItemOfType( const ID::VTX_ID & p_id ) const
	{
		for ( const Model::ID & modelID : getItems() )
		{
			if ( MVC::MvcManager::get().getModelTypeID( modelID ) == p_id )
			{
				return true;
			}
		}

		return false;
	}

	bool Selection::hasMolecule() const { return _moleculesMap.size() > 0; }

	void Selection::selectMolecule( Molecule & p_molecule, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();
		_selectMolecule( p_molecule );
		_setCurrentObject( &p_molecule );
		p_molecule.refreshSelection( &_moleculesMap[ p_molecule.getId() ] );
		_notifyDataChanged();
		chrono.stop();
		VTX_DEBUG( "Selection time: " + std::to_string( chrono.elapsedTime() ) );
	}
	void Selection::selectMolecules( const std::vector<Molecule *> & p_molecules,
									 const bool						 p_appendToSelection,
									 const Molecule * const			 p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		else if ( p_molecules.size() <= 0 ) // No modifications
			return;

		for ( const auto it : p_molecules )
		{
			_selectMolecule( *it );

			it->refreshSelection( &_moleculesMap[ it->getId() ] );
		}

		if ( p_currentObj == nullptr )
		{
			if ( p_molecules.size() > 0 )
				_setCurrentObject( *p_molecules.crbegin() );
		}
		else
		{
			_setCurrentObject( p_currentObj );
		}

		_notifyDataChanged();
	}

	void Selection::selectChain( Chain & p_chain, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectChain( p_chain );
		_setCurrentObject( &p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::selectChains( const std::vector<Chain *> & p_chains,
								  const bool				   p_appendToSelection,
								  const Chain * const		   p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();
		else if ( p_chains.size() <= 0 )
			return;

		for ( auto it : p_chains )
			_selectChain( *it );

		if ( p_currentObj == nullptr )
		{
			if ( p_chains.size() > 0 )
				_setCurrentObject( *p_chains.crbegin() );
		}
		else
		{
			_setCurrentObject( p_currentObj );
		}
		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void Selection::selectResidue( Residue & p_residue, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectResidue( p_residue );
		_setCurrentObject( &p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::selectResidues( const std::vector<Residue *> & p_residues,
									const bool					   p_appendToSelection,
									const Residue * const		   p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();
		else if ( p_residues.size() <= 0 )
			return;

		if ( p_residues.size() == 0 )
			return;

		for ( const auto it : p_residues )
			_selectResidue( *it );

		if ( p_currentObj == nullptr )
		{
			if ( p_residues.size() > 0 )
				_setCurrentObject( *p_residues.crbegin() );
		}
		else
		{
			_setCurrentObject( p_currentObj );
		}

		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void Selection::selectAtom( Atom & p_atom, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectAtom( p_atom );
		_setCurrentObject( &p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::selectAtoms( const std::vector<Atom *> & p_atoms,
								 const bool					 p_appendToSelection,
								 const Atom * const			 p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();
		else if ( p_atoms.size() <= 0 )
			return;

		if ( p_atoms.size() == 0 )
			return;

		for ( const auto it : p_atoms )
			_selectAtom( *it );

		if ( p_currentObj == nullptr )
		{
			if ( p_atoms.size() > 0 )
				_setCurrentObject( *p_atoms.crbegin() );
		}
		else
		{
			_setCurrentObject( p_currentObj );
		}

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void Selection::unselectMolecule( Molecule & p_molecule )
	{
		_unselectMolecule( p_molecule );
		_refreshMoleculeSelection( &p_molecule );
		_notifyDataChanged();
	}
	void Selection::unselectMolecules( const std::vector<Molecule *> & p_molecules )
	{
		if ( p_molecules.size() == 0 )
			return;

		for ( const auto it : p_molecules )
		{
			_unselectMolecule( *it );
			_refreshMoleculeSelection( it );
		}

		_notifyDataChanged();
	}
	void Selection::unselectMoleculesWithCheck( const std::vector<Molecule *> & p_molecules )
	{
		for ( const auto it : p_molecules )
		{
			Model::Molecule & molecule = *it;
			if ( isMoleculeSelected( molecule ) )
			{
				_unselectMolecule( molecule );
				molecule.refreshSelection( nullptr );
			}
		}

		_notifyDataChanged();
	}

	void Selection::unselectChain( Chain & p_chain )
	{
		_unselectChain( p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectChains( const std::vector<Chain *> & p_chains )
	{
		if ( p_chains.size() == 0 )
			return;

		for ( const auto it : p_chains )
			_unselectChain( *it );

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectChainsWithCheck( const std::vector<Chain *> & p_chains )
	{
		for ( const auto it : p_chains )
		{
			if ( isChainSelected( *it ) )
				_unselectChain( *it );
		}

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void Selection::unselectResidue( Residue & p_residue )
	{
		_unselectResidue( p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectResidues( const std::vector<Residue *> & p_residues )
	{
		if ( p_residues.size() == 0 )
			return;

		for ( const auto it : p_residues )
			_unselectResidue( *it );

		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectResiduesWithCheck( const std::vector<Residue *> & p_residues )
	{
		for ( const auto it : p_residues )
		{
			if ( isResidueSelected( *it ) )
				_unselectResidue( *it );
		}
		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void Selection::unselectAtom( Atom & p_atom )
	{
		_unselectAtom( p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectAtoms( const std::vector<Atom *> & p_atoms )
	{
		if ( p_atoms.size() == 0 )
			return;

		for ( const auto it : p_atoms )
			_unselectAtom( *it );

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void Selection::unselectAtomsWithCheck( const std::vector<Atom *> & p_atoms )
	{
		for ( const auto it : p_atoms )
		{
			if ( isAtomSelected( *it ) )
				_unselectAtom( *it );
		}

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	bool Selection::isMoleculeSelected( const Molecule & p_molecule ) const
	{
		const ID & id = p_molecule.getId();
		return _items.find( id ) != _items.end();
	}
	bool Selection::isMoleculeFullySelected( const Molecule & p_molecule ) const
	{
		const ID &							 id = p_molecule.getId();
		const MapMoleculeIds::const_iterator it = _moleculesMap.find( id );

		return _items.find( id ) != _items.end()
			   && it->second.getFullySelectedChildCount() == p_molecule.getRealChainCount();
	}
	bool Selection::isChainSelected( const Chain & p_chain ) const
	{
		const ID & moleculeId = p_chain.getMoleculePtr()->getId();

		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap = _moleculesMap.at( moleculeId );
		const uint &		index	 = p_chain.getIndex();

		return chainMap.find( index ) != chainMap.end();
	}
	bool Selection::isChainFullySelected( const Chain & p_chain ) const
	{
		const ID & moleculeId = p_chain.getMoleculePtr()->getId();
		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds &				  chainMap = _moleculesMap.at( moleculeId );
		const MapChainIds::const_iterator it	   = chainMap.find( p_chain.getIndex() );

		return it != chainMap.end() && it->second.getFullySelectedChildCount() == p_chain.getRealResidueCount();
	}
	bool Selection::isResidueSelected( const Residue & p_residue ) const
	{
		const ID & moleculeId = p_residue.getMoleculePtr()->getId();

		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap   = _moleculesMap.at( moleculeId );
		const uint &		chainIndex = p_residue.getChainPtr()->getIndex();

		if ( chainMap.find( chainIndex ) == chainMap.end() )
			return false;

		const MapResidueIds & residueMap = chainMap.at( chainIndex );
		const uint &		  index		 = p_residue.getIndex();

		return residueMap.find( index ) != residueMap.end();
	}
	bool Selection::isResidueFullySelected( const Residue & p_residue ) const
	{
		const ID & moleculeId = p_residue.getMoleculePtr()->getId();
		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap   = _moleculesMap.at( moleculeId );
		const uint &		chainIndex = p_residue.getChainPtr()->getIndex();

		if ( chainMap.find( chainIndex ) == chainMap.end() )
			return false;

		const MapResidueIds &				residueMap = chainMap.at( chainIndex );
		const MapResidueIds::const_iterator it		   = residueMap.find( p_residue.getIndex() );

		return it != residueMap.end() && it->second.getFullySelectedChildCount() == p_residue.getRealAtomCount();
	}
	bool Selection::isAtomSelected( const Atom & p_atom ) const
	{
		const ID & moleculeId = p_atom.getMoleculePtr()->getId();

		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap   = _moleculesMap.at( moleculeId );
		const uint &		chainIndex = p_atom.getChainPtr()->getIndex();

		if ( chainMap.find( chainIndex ) == chainMap.end() )
			return false;

		const MapResidueIds & residueMap   = _moleculesMap.at( moleculeId ).at( chainIndex );
		const uint &		  residueIndex = p_atom.getResiduePtr()->getIndex();
		if ( residueMap.find( residueIndex ) == residueMap.end() )
			return false;

		const VecAtomIds & atomVector = _moleculesMap.at( moleculeId ).at( chainIndex ).at( residueIndex );
		const uint &	   index	  = p_atom.getIndex();

		return std::find( atomVector.begin(), atomVector.end(), index ) != atomVector.end();
	}

	uint Selection::getMoleculeSelectedCount() const { return (uint)_moleculesMap.size(); }
	uint Selection::getChainSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<ID, MapChainIds> mapMolecules : _moleculesMap )
			res += (uint)mapMolecules.second.size();
		return res;
	}
	uint Selection::getResidueSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<ID, MapChainIds> mapMolecules : _moleculesMap )
			for ( const std::pair<ID, MapResidueIds> mapChains : mapMolecules.second )
				res += (uint)mapChains.second.size();
		return res;
	}
	uint Selection::getAtomSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<ID, MapChainIds> mapMolecules : _moleculesMap )
			for ( const std::pair<ID, MapResidueIds> mapChains : mapMolecules.second )
				for ( const std::pair<ID, VecAtomIds> mapResidus : mapChains.second )
					res += (uint)mapResidus.second.size();
		return res;
	}

	void Selection::_selectMolecule( const Molecule & p_molecule )
	{
		_addMolecule( p_molecule );
		_addMoleculeContent( p_molecule );

		_mapSelectionAABB[ p_molecule.getId() ].extend( p_molecule.getWorldAABB() );
	}

	void Selection::_selectChain( const Chain & p_chain )
	{
		const Molecule & parent = *p_chain.getMoleculePtr();
		_addMolecule( parent );
		_addChain( p_chain );
		_addChainContent( p_chain );

		_mapSelectionAABB[ parent.getId() ].extend( p_chain.getWorldAABB() );
	}

	void Selection::_selectResidue( const Residue & p_residue )
	{
		const Chain &	 chainParent	= *p_residue.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( p_residue );
		_addResidueContent( p_residue );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_residue.getWorldAABB() );
	}

	void Selection::_selectAtom( const Atom & p_atom )
	{
		const Residue &	 residueParent	= *p_atom.getResiduePtr();
		const Chain &	 chainParent	= *residueParent.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( residueParent );
		const bool atomAdded = _addAtom( p_atom );

		if ( atomAdded )
			_referenceAtom( p_atom );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_atom.getWorldAABB() );
	}

	void Selection::_referenceAtom( const Atom & p_atom )
	{
		const Residue &	 residueParent	= *p_atom.getResiduePtr();
		const Chain &	 chainParent	= *residueParent.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		atoms._addFullChild();
		if ( atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount() )
			_referenceFullResidue( residueParent );
	}
	void Selection::_referenceFullResidue( const Residue & p_residue )
	{
		const Chain &	 chainParent	= *p_residue.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		residues._addFullChild();
		if ( residues.getFullySelectedChildCount() == chainParent.getRealResidueCount() )
			_referenceFullChain( chainParent );
	}
	void Selection::_referenceFullChain( const Chain & p_chain )
	{
		const Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._addFullChild();
	}

	void Selection::_unreferenceAtom( const Atom & p_atom )
	{
		const Residue &	 residueParent	= *p_atom.getResiduePtr();
		const Chain &	 chainParent	= *residueParent.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		const bool propagateToParent = atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount();
		atoms._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullResidue( residueParent );
	}
	void Selection::_unreferenceFullResidue( const Residue & p_residue )
	{
		const Chain &	 chainParent	= *p_residue.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues		  = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		const bool		propagateToParent = residues.getFullySelectedChildCount() == chainParent.getRealResidueCount();
		residues._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullChain( chainParent );
	}
	void Selection::_unreferenceFullChain( const Chain & p_chain )
	{
		const Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._removeFullChild();
	}

	void Selection::_unselectMolecule( const Molecule & p_molecule )
	{
		_removeMolecule( p_molecule );
		_recomputeAABB();
	}

	void Selection::_unselectChain( const Chain & p_chain )
	{
		_removeChain( p_chain );
		_recomputeAABB();
	}

	void Selection::_unselectResidue( const Residue & p_residue )
	{
		_removeResidue( p_residue );
		_recomputeAABB();
	}

	void Selection::_unselectAtom( const Atom & p_atom )
	{
		_removeAtom( p_atom );
		_recomputeAABB();
	}

	bool Selection::_addMolecule( const Molecule & p_molecule )
	{
		const ID & id				= p_molecule.getId();
		const bool hasToAddMolecule = _items.find( id ) == _items.end();

		if ( hasToAddMolecule )
		{
			_emplaceMolecule( p_molecule );
		}

		return hasToAddMolecule;
	}

	bool Selection::_addChain( const Chain & p_chain )
	{
		const ID &	 moleculeId = p_chain.getMoleculePtr()->getId();
		const uint & index		= p_chain.getIndex();

		MapChainIds & chainMap = _moleculesMap[ moleculeId ];

		const bool hasToAddChain = chainMap.find( index ) == chainMap.end();

		if ( hasToAddChain )
		{
			chainMap.emplace( index, MapResidueIds() );
		}

		return hasToAddChain;
	}

	bool Selection::_addResidue( const Residue & p_residue )
	{
		const ID &	 moleculeId = p_residue.getMoleculePtr()->getId();
		const uint & chainIndex = p_residue.getChainPtr()->getIndex();
		const uint & index		= p_residue.getIndex();

		MapResidueIds & residueMap = _moleculesMap[ moleculeId ][ chainIndex ];

		const bool hasToAddResidue = residueMap.find( index ) == residueMap.end();

		if ( hasToAddResidue )
		{
			residueMap.emplace( index, VecAtomIds() );
		}

		return hasToAddResidue;
	}

	bool Selection::_addAtom( const Atom & p_atom )
	{
		const ID &	 moleculeId	  = p_atom.getMoleculePtr()->getId();
		const uint & chainIndex	  = p_atom.getChainPtr()->getIndex();
		const uint & residueIndex = p_atom.getResiduePtr()->getIndex();
		const uint & index		  = p_atom.getIndex();

		VecAtomIds & atomVector = _moleculesMap[ moleculeId ][ chainIndex ][ residueIndex ];

		const bool hasToAddAtom = std::find( atomVector.begin(), atomVector.end(), index ) == atomVector.end();
		if ( hasToAddAtom )
		{
			atomVector.emplace_back( index );
		}

		return hasToAddAtom;
	}

	void Selection::_addMoleculeContent( const Molecule & p_molecule )
	{
		//_items[ p_molecule.getId() ]._setFullChildrenCount( p_molecule.getChainCount() );

		for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
		{
			const Chain * const chain = p_molecule.getChain( i );

			if ( chain == nullptr )
				continue;

			_addChain( *chain );
			_addChainContent( *chain );
		}
	}

	void Selection::_addChainContent( const Chain & p_chain )
	{
		const Molecule & moleculeParent = *p_chain.getMoleculePtr();
		//_referenceFullChain( p_chain );

		const ID &		moleculeId = moleculeParent.getId();
		const uint		chainIndex = p_chain.getIndex();
		MapResidueIds & residueMap = _moleculesMap[ moleculeId ][ chainIndex ];

		if ( residueMap.getFullySelectedChildCount() == p_chain.getRealResidueCount() )
			return;

		for ( uint i = 0; i < p_chain.getResidueCount(); ++i )
		{
			const Residue * const residue = moleculeParent.getResidue( p_chain.getIndexFirstResidue() + i );

			if ( residue == nullptr )
				continue;

			//_addResidue( *residue );
			const uint index		   = residue->getIndex();
			const bool hasToAddResidue = residueMap.find( index ) == residueMap.end();

			if ( hasToAddResidue )
			{
				residueMap.emplace( index, VecAtomIds() );
			}

			_addResidueContent( *residue );
		}
	}

	void Selection::_addResidueContent( const Residue & p_residue )
	{
		const Chain &	 chainParent	= *p_residue.getChainPtr();
		const Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ p_residue.getIndex() ];

		// All atoms already added
		if ( atoms.getFullySelectedChildCount() >= p_residue.getRealAtomCount() )
			return;

		atoms._setFullChildrenCount( p_residue.getRealAtomCount() );
		_referenceFullResidue( p_residue );

		size_t nextAtomIndex = atoms.size();
		atoms.resize( p_residue.getRealAtomCount(), INVALID_ID );

		VecAtomIds::const_iterator itLastAtomFindCheck = atoms.cbegin() + nextAtomIndex;

		const uint firstAtom	 = p_residue.getIndexFirstAtom();
		const uint nextFirstAtom = firstAtom + p_residue.getAtomCount();

		for ( uint i = firstAtom; i < nextFirstAtom; ++i )
		{
			const Atom * const atom = moleculeParent.getAtom( i );

			if ( atom == nullptr )
				continue;

			//_addAtom( *atom );

			const uint atomIndex = atom->getIndex();

			const bool hasToAddAtom
				= std::find( atoms.cbegin(), itLastAtomFindCheck, atomIndex ) == itLastAtomFindCheck;

			if ( hasToAddAtom )
			{
				atoms[ nextAtomIndex ] = atom->getIndex();
				nextAtomIndex++;
			}
		}
	}

	void Selection::_removeMolecule( const Molecule & p_molecule )
	{
		const ID & id = p_molecule.getId();
		_moleculesMap.erase( id );
		_items.erase( id );
		_mapSelectionAABB.erase( id );
	}

	void Selection::_removeChain( const Chain & p_chain )
	{
		const ID & moleculeId = p_chain.getMoleculePtr()->getId();
		const ID & index	  = p_chain.getIndex();

		_moleculesMap[ moleculeId ].erase( index );

		if ( _moleculesMap[ moleculeId ].size() == 0 )
		{
			_removeMolecule( *p_chain.getMoleculePtr() );
		}
		else
		{
			_unreferenceFullChain( p_chain );
		}
	}

	void Selection::_removeResidue( const Residue & p_residue )
	{
		const ID & moleculeId = p_residue.getMoleculePtr()->getId();
		const ID & chainIndex = p_residue.getChainPtr()->getIndex();
		const ID & index	  = p_residue.getIndex();

		_moleculesMap[ moleculeId ][ chainIndex ].erase( index );

		if ( _moleculesMap[ moleculeId ][ chainIndex ].size() == 0 )
		{
			_removeChain( *p_residue.getChainPtr() );
		}
		else
		{
			_unreferenceFullResidue( p_residue );
		}
	}

	void Selection::_removeAtom( const Atom & p_atom )
	{
		const ID & moleculeId	= p_atom.getMoleculePtr()->getId();
		const ID & chainIndex	= p_atom.getChainPtr()->getIndex();
		const ID & residueIndex = p_atom.getResiduePtr()->getIndex();
		const ID & index		= p_atom.getIndex();

		VecAtomIds & atomVector = _moleculesMap[ moleculeId ][ chainIndex ][ residueIndex ];

		atomVector.erase( std::remove( atomVector.begin(), atomVector.end(), index ), atomVector.end() );
		if ( atomVector.size() == 0 )
		{
			_removeResidue( *p_atom.getResiduePtr() );
		}
		else
		{
			_unreferenceAtom( p_atom );
		}
	}

	void Selection::_refreshMoleculeSelection( Molecule * const p_molecule )
	{
		if ( _moleculesMap.find( p_molecule->getId() ) != _moleculesMap.end() )
		{
			p_molecule->refreshSelection( &_moleculesMap[ p_molecule->getId() ] );
		}
		else
		{
			p_molecule->refreshSelection( nullptr );
		}
	}

	void Selection::selectModels( const std::vector<Model::Molecule *> & p_molecules,
								  const std::vector<Model::Chain *> &	 p_chains,
								  const std::vector<Model::Residue *> &	 p_residues,
								  const std::vector<Model::Atom *> &	 p_atoms,
								  const bool							 p_appendToSelection,
								  const Model::BaseModel * const		 p_currentObj )
	{
		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		std::unordered_set<Model::Molecule *> moleculeSet = std::unordered_set<Model::Molecule *>();

		for ( Model::Molecule * const it : p_molecules )
		{
			_selectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const Model::Chain * const it : p_chains )
		{
			_selectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const Model::Residue * const it : p_residues )
		{
			_selectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const Model::Atom * const it : p_atoms )
		{
			_selectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( Model::Molecule * const it : moleculeSet )
		{
			_emplaceMolecule( *it );
			it->refreshSelection( &_moleculesMap[ it->getId() ] );
		}

		if ( p_currentObj == nullptr )
		{
			if ( p_molecules.size() > 0 )
				_setCurrentObject( *p_molecules.crbegin() );
			else if ( p_chains.size() > 0 )
				_setCurrentObject( *p_chains.crbegin() );
			else if ( p_residues.size() > 0 )
				_setCurrentObject( *p_residues.crbegin() );
			else if ( p_atoms.size() > 0 )
				_setCurrentObject( *p_atoms.crbegin() );
		}
		else
		{
			_setCurrentObject( p_currentObj );
		}

		_notifyDataChanged();
		chrono.stop();
		VTX_DEBUG( "Selection time: " + std::to_string( chrono.elapsedTime() ) );
	}
	void Selection::unselectModels( const std::vector<Model::Molecule *> & p_molecules,
									const std::vector<Model::Chain *> &	   p_chains,
									const std::vector<Model::Residue *> &  p_residus,
									const std::vector<Model::Atom *> &	   p_atoms )
	{
		std::unordered_set<Model::Molecule *> moleculeSet = std::unordered_set<Model::Molecule *>();

		for ( Model::Molecule * const it : p_molecules )
		{
			_unselectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const Model::Chain * const it : p_chains )
		{
			_unselectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const Model::Residue * const it : p_residus )
		{
			_unselectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const Model::Atom * const it : p_atoms )
		{
			_unselectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( Model::Molecule * const it : moleculeSet )
		{
			_refreshMoleculeSelection( it );
		}

		_notifyDataChanged();
	}

	void Selection::selectModel( Model::BaseModel & p_model, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		const ID::VTX_ID & modelType = p_model.getTypeId();

		if ( modelType == ID::Model::MODEL_MOLECULE )
			selectMolecule( static_cast<Model::Molecule &>( p_model ), p_appendToSelection );
		else if ( modelType == ID::Model::MODEL_CHAIN )
			selectChain( static_cast<Model::Chain &>( p_model ), p_appendToSelection );
		else if ( modelType == ID::Model::MODEL_RESIDUE )
			selectResidue( static_cast<Model::Residue &>( p_model ), p_appendToSelection );
		else if ( modelType == ID::Model::MODEL_ATOM )
			selectAtom( static_cast<Model::Atom &>( p_model ), p_appendToSelection );
		else
		{
			_items.emplace( p_model.getId() );
			_notifyDataChanged();
		}
	}

	void Selection::unselectModel( const Model::BaseModel & p_model )
	{
		_unselectModel( p_model );
		_notifyDataChanged();
	}
	void Selection::_unselectModel( const Model::BaseModel & p_model ) { _items.erase( p_model.getId() ); }

	bool Selection::isModelSelected( const Model::BaseModel & p_model ) const
	{
		const ID::VTX_ID & modelTypeID = p_model.getTypeId();

		if ( modelTypeID == ID::Model::MODEL_CHAIN )
			return isChainSelected( static_cast<const Model::Chain &>( p_model ) );
		else if ( modelTypeID == ID::Model::MODEL_RESIDUE )
			return isResidueSelected( static_cast<const Model::Residue &>( p_model ) );
		else if ( modelTypeID == ID::Model::MODEL_ATOM )
			return isAtomSelected( static_cast<const Model::Atom &>( p_model ) );
		else
			return _items.find( p_model.getId() ) != _items.end();
	}

	bool Selection::isEmpty() const { return _items.size() <= 0; }

	void Selection::clear()
	{
		const Model::BaseModel * const previousCurrentObject = _currentObject;
		_clearWithoutNotify();

		_notifyDataChanged();

		if ( previousCurrentObject != _currentObject )
			VTX_EVENT( new Event::VTXEventPtr( Event::CURRENT_ITEM_IN_SELECTION_CHANGE, _currentObject ) );
	}
	void Selection::_clearWithoutNotify()
	{
		for ( const std::pair<ID, MapChainIds> item : _moleculesMap )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( item.first );
			molecule.refreshSelection( nullptr );
		}

		_items.clear();
		_moleculesMap.clear();
		_mapSelectionAABB.clear();
		_clearCurrentObject( false );
	}

	void Selection::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::MOLECULE_REMOVED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
			unselectMolecule( *castedEvent.ptr );
		}
	}

	void Selection::_notifyDataChanged()
	{
		BaseModel::_notifyDataChanged();
		VTX_EVENT( new Event::VTXEventPtr( Event ::SELECTION_CHANGE, this ) );
	}

	void Selection::getItemTypes( std::set<ID::VTX_ID> & p_types ) const
	{
		p_types.clear();

		for ( const Model::ID & id : _items )
		{
			const ID::VTX_ID & typeId = MVC::MvcManager::get().getModelTypeID( id );

			if ( typeId == ID::Model::MODEL_MOLECULE )
			{
				const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( id );

				if ( isMoleculeFullySelected( molecule ) )
				{
					p_types.emplace( molecule.getTypeId() );
					continue;
				}

				for ( const std::pair<Model::ID, MapResidueIds> & chainData : _moleculesMap.at( id ) )
				{
					const Model::Chain & chain = *molecule.getChain( chainData.first );
					if ( isChainFullySelected( chain ) )
					{
						p_types.emplace( chain.getTypeId() );
						continue;
					}

					for ( const std::pair<Model::ID, VecAtomIds> & residueData : chainData.second )
					{
						const Model::Residue & residue = *molecule.getResidue( residueData.first );
						if ( isResidueFullySelected( residue ) )
						{
							p_types.emplace( residue.getTypeId() );
						}
						else
						{
							p_types.emplace( ID::Model::MODEL_ATOM );
						}
					}
				}
			}
			else
			{
				p_types.emplace( typeId );
			}
		}
	}

	Math::AABB Selection::getAABB() const
	{
		Math::AABB res = Math::AABB();

		for ( const std::pair<Model::ID, Math::AABB> & pairIdAabb : _mapSelectionAABB )
		{
			const Model::Molecule & molecule	= MVC::MvcManager::get().getModel<Model::Molecule>( pairIdAabb.first );
			const Math::AABB &		currentAABB = pairIdAabb.second;
			res.extend( currentAABB );
		}

		return res;
	}

	void Selection::_recomputeAABB()
	{
		_mapSelectionAABB.clear();

		for ( const std::pair<ID, MapChainIds> & mapMol : _moleculesMap )
		{
			const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( mapMol.first );
			Math::AABB				aabb	 = Math::AABB();
			_mapSelectionAABB.emplace( molecule.getId(), aabb );

			if ( molecule.getChainCount() == mapMol.second.size() )
			{
				aabb.extend( molecule.getAABB() );
				continue;
			}

			for ( const std::pair<ID, MapResidueIds> & mapChain : mapMol.second )
			{
				const Model::Chain & chain = *molecule.getChain( mapChain.first );
				if ( chain.getResidueCount() == mapChain.second.size() )
				{
					aabb.extend( chain.getAABB() );
					continue;
				}

				for ( const std::pair<ID, VecAtomIds> & mapResidue : mapChain.second )
				{
					const Model::Residue & residue = *molecule.getResidue( mapResidue.first );
					if ( residue.getAtomCount() == mapResidue.second.size() )
					{
						aabb.extend( residue.getAABB() );
						continue;
					}

					for ( const uint & vectorId : mapResidue.second )
					{
						const Model::Atom & atom = *molecule.getAtom( vectorId );
						aabb.extend( atom.getAABB() );
					}
				}
			}
		}
	}
	const Model::BaseModel * const Selection::getCurrentObject() const { return _currentObject; }

	void Selection::_emplaceMolecule( const Molecule & p_molecule )
	{
		_items.emplace( p_molecule.getId() );
		_moleculesMap.emplace( p_molecule.getId(), MapChainIds() );
	}

	void Selection::_setCurrentObject( const Model::BaseModel * const p_model, const bool p_notify )
	{
		if ( _currentObject != p_model )
		{
			_currentObject = p_model;

			if ( p_notify )
				VTX_EVENT( new Event::VTXEventPtr( Event::CURRENT_ITEM_IN_SELECTION_CHANGE, _currentObject ) );
		}
	}
	void Selection::_clearCurrentObject( const bool p_notify )
	{
		if ( _currentObject != nullptr )
		{
			_currentObject = nullptr;
			if ( p_notify )
				VTX_EVENT( new Event::VTXEventPtr( Event::CURRENT_ITEM_IN_SELECTION_CHANGE, _currentObject ) );
		}
	}

} // namespace VTX::Model
