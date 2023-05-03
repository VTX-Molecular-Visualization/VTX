#include "app/application/selection/selection.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/event.hpp"
#include "app/mvc.hpp"
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/constants.hpp>
#include <util/logger.hpp>

namespace VTX::App::Application::Selection
{
	bool SelectionModel::hasItemOfType( const VTX::App::VTX_ID & p_id ) const
	{
		for ( const App::Core::Model::ID & modelID : getItems() )
		{
			if ( VTX::MVC_MANAGER().getModelTypeID( modelID ) == p_id )
			{
				return true;
			}
		}

		return false;
	}

	const SelectionModel::MapChainIds * const SelectionModel::getMoleculeMap(
		const App::Component::Chemistry::Molecule & p_molecule )
	{
		if ( _moleculesMap.find( p_molecule.getId() ) != _moleculesMap.end() )
		{
			return &_moleculesMap[ p_molecule.getId() ];
		}
		else
		{
			return nullptr;
		}
	}

	bool SelectionModel::hasMolecule() const { return _moleculesMap.size() > 0; }

	void SelectionModel::selectMolecule( App::Component::Chemistry::Molecule & p_molecule,
										 const bool							   p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		Util::Chrono chrono = Util::Chrono();
		chrono.start();
		_selectMolecule( p_molecule );
		_setCurrentObject( &p_molecule );
		p_molecule.refreshSelection( &_moleculesMap[ p_molecule.getId() ] );
		_notifyDataChanged();
		chrono.stop();
	}
	void SelectionModel::selectMolecules( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
										  const bool										p_appendToSelection,
										  const App::Component::Chemistry::Molecule * const p_currentObj )
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
	void SelectionModel::selectCategory( App::Component::Chemistry::Category & p_category,
										 const bool							   p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectCategory( p_category );
		_setCurrentObject( &p_category );
		_refreshMoleculeSelection( p_category.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectCategories( const std::vector<App::Component::Chemistry::Category *> & p_categories,
										   const bool										 p_appendToSelection,
										   const App::Component::Chemistry::Category * const p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();
		else if ( p_categories.size() <= 0 )
			return;

		for ( App::Component::Chemistry::Category * const categoryPtr : p_categories )
			_selectCategory( *categoryPtr );

		const App::Component::Chemistry::Category * const currentObj
			= p_currentObj != nullptr ? p_currentObj : *p_categories.crbegin();
		_setCurrentObject( currentObj );

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectChain( App::Component::Chemistry::Chain & p_chain, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectChain( p_chain );
		_setCurrentObject( &p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectChains( const std::vector<App::Component::Chemistry::Chain *> & p_chains,
									   const bool											   p_appendToSelection,
									   const App::Component::Chemistry::Chain * const		   p_currentObj )
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

	void SelectionModel::selectResidue( App::Component::Chemistry::Residue & p_residue, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectResidue( p_residue );
		_setCurrentObject( &p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectResidues( const std::vector<App::Component::Chemistry::Residue *> & p_residues,
										 const bool												   p_appendToSelection,
										 const App::Component::Chemistry::Residue * const		   p_currentObj )
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

	void SelectionModel::selectAtom( App::Component::Chemistry::Atom & p_atom, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectAtom( p_atom );
		_setCurrentObject( &p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectAtoms( const std::vector<App::Component::Chemistry::Atom *> & p_atoms,
									  const bool											 p_appendToSelection,
									  const App::Component::Chemistry::Atom * const			 p_currentObj )
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

	void SelectionModel::unselectMolecule( App::Component::Chemistry::Molecule & p_molecule )
	{
		_unselectMolecule( p_molecule );
		_refreshMoleculeSelection( &p_molecule );
		_notifyDataChanged();
	}
	void SelectionModel::unselectMolecules( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules )
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
	void SelectionModel::unselectMoleculesWithCheck(
		const std::vector<App::Component::Chemistry::Molecule *> & p_molecules )
	{
		for ( const auto it : p_molecules )
		{
			App::Component::Chemistry::Molecule & molecule = *it;
			if ( isMoleculeSelected( molecule ) )
			{
				_unselectMolecule( molecule );
				molecule.refreshSelection( nullptr );
			}
		}

		_notifyDataChanged();
	}

	void SelectionModel::unselectCategory( App::Component::Chemistry::Category & p_category )
	{
		_unselectCategory( p_category );
		_refreshMoleculeSelection( p_category.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectCategories( const std::vector<App::Component::Chemistry::Category *> & p_categories )
	{
		if ( p_categories.size() == 0 )
			return;

		for ( const App::Component::Chemistry::Category * const it : p_categories )
			_unselectCategory( *it );

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectCategoriesWithCheck(
		const std::vector<App::Component::Chemistry::Category *> & p_categories )
	{
		if ( p_categories.size() == 0 )
			return;

		for ( const App::Component::Chemistry::Category * const it : p_categories )
		{
			if ( isCategorySelected( *it ) )
				_unselectCategory( *it );
		}

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChain( App::Component::Chemistry::Chain & p_chain )
	{
		_unselectChain( p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChains( const std::vector<App::Component::Chemistry::Chain *> & p_chains )
	{
		if ( p_chains.size() == 0 )
			return;

		for ( const auto it : p_chains )
			_unselectChain( *it );

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChainsWithCheck( const std::vector<App::Component::Chemistry::Chain *> & p_chains )
	{
		for ( const auto it : p_chains )
		{
			if ( isChainSelected( *it ) )
				_unselectChain( *it );
		}

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void SelectionModel::unselectResidue( App::Component::Chemistry::Residue & p_residue )
	{
		_unselectResidue( p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectResidues( const std::vector<App::Component::Chemistry::Residue *> & p_residues )
	{
		if ( p_residues.size() == 0 )
			return;

		for ( const auto it : p_residues )
			_unselectResidue( *it );

		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectResiduesWithCheck(
		const std::vector<App::Component::Chemistry::Residue *> & p_residues )
	{
		for ( const auto it : p_residues )
		{
			if ( isResidueSelected( *it ) )
				_unselectResidue( *it );
		}
		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void SelectionModel::unselectAtom( App::Component::Chemistry::Atom & p_atom )
	{
		_unselectAtom( p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectAtoms( const std::vector<App::Component::Chemistry::Atom *> & p_atoms )
	{
		if ( p_atoms.size() == 0 )
			return;

		for ( const auto it : p_atoms )
			_unselectAtom( *it );

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectAtomsWithCheck( const std::vector<App::Component::Chemistry::Atom *> & p_atoms )
	{
		for ( const auto it : p_atoms )
		{
			if ( isAtomSelected( *it ) )
				_unselectAtom( *it );
		}

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	bool SelectionModel::isMoleculeSelected( const App::Component::Chemistry::Molecule & p_molecule ) const
	{
		const App::Core::Model::ID & id = p_molecule.getId();
		return _items.find( id ) != _items.end();
	}
	bool SelectionModel::isMoleculeFullySelected( const App::Component::Chemistry::Molecule & p_molecule ) const
	{
		const App::Core::Model::ID &		 id = p_molecule.getId();
		const MapMoleculeIds::const_iterator it = _moleculesMap.find( id );

		return _items.find( id ) != _items.end()
			   && it->second.getFullySelectedChildCount() == p_molecule.getRealChainCount();
	}
	bool SelectionModel::isChainSelected( const App::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();

		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap = _moleculesMap.at( moleculeId );
		const uint &		index	 = p_chain.getIndex();

		return chainMap.find( index ) != chainMap.end();
	}
	bool SelectionModel::isChainFullySelected( const App::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds &				  chainMap = _moleculesMap.at( moleculeId );
		const MapChainIds::const_iterator it	   = chainMap.find( p_chain.getIndex() );

		return it != chainMap.end() && it->second.getFullySelectedChildCount() == p_chain.getRealResidueCount();
	}
	bool SelectionModel::isResidueSelected( const App::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();

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
	bool SelectionModel::isResidueFullySelected( const App::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
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
	bool SelectionModel::isAtomSelected( const App::Component::Chemistry::Atom & p_atom ) const
	{
		const App::Core::Model::ID & moleculeId = p_atom.getMoleculePtr()->getId();

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

	uint SelectionModel::getMoleculeSelectedCount() const { return (uint)_moleculesMap.size(); }
	uint SelectionModel::getChainSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			res += (uint)mapMolecules.second.size();
		return res;
	}
	uint SelectionModel::getResidueSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			for ( const std::pair<const VTX::App::Core::Model::ID, MapResidueIds> & mapChains : mapMolecules.second )
				res += (uint)mapChains.second.size();
		return res;
	}
	uint SelectionModel::getAtomSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			for ( const std::pair<const VTX::App::Core::Model::ID, MapResidueIds> & mapChains : mapMolecules.second )
				for ( const std::pair<const VTX::App::Core::Model::ID, VecAtomIds> & mapResidus : mapChains.second )
					res += (uint)mapResidus.second.size();
		return res;
	}

	void SelectionModel::_selectMolecule( const App::Component::Chemistry::Molecule & p_molecule )
	{
		_addMolecule( p_molecule );
		_addMoleculeContent( p_molecule );

		_mapSelectionAABB[ p_molecule.getId() ].extend( p_molecule.getWorldAABB() );
	}

	void SelectionModel::_selectCategory( const App::Component::Chemistry::Category & p_category )
	{
		App::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( const uint chainIndex : p_category.getChains() )
		{
			App::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			_selectChain( *chain );
		}
	}

	void SelectionModel::_selectChain( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Component::Chemistry::Molecule & parent = *p_chain.getMoleculePtr();
		_addMolecule( parent );
		_addChain( p_chain );
		_addChainContent( p_chain );

		_mapSelectionAABB[ parent.getId() ].extend( p_chain.getWorldAABB() );
	}

	void SelectionModel::_selectResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( p_residue );
		_addResidueContent( p_residue );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_residue.getWorldAABB() );
	}

	void SelectionModel::_selectAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		const App::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( residueParent );
		const bool atomAdded = _addAtom( p_atom );

		if ( atomAdded )
			_referenceAtom( p_atom );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_atom.getWorldAABB() );
	}

	void SelectionModel::_referenceAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		const App::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		atoms._addFullChild();
		if ( atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount() )
			_referenceFullResidue( residueParent );
	}
	void SelectionModel::_referenceFullResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		residues._addFullChild();
		if ( residues.getFullySelectedChildCount() == chainParent.getRealResidueCount() )
			_referenceFullChain( chainParent );
	}
	void SelectionModel::_referenceFullChain( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._addFullChild();
	}

	void SelectionModel::_unreferenceAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		const App::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		const bool propagateToParent = atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount();
		atoms._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullResidue( residueParent );
	}
	void SelectionModel::_unreferenceFullResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues		  = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		const bool		propagateToParent = residues.getFullySelectedChildCount() == chainParent.getRealResidueCount();
		residues._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullChain( chainParent );
	}
	void SelectionModel::_unreferenceFullChain( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._removeFullChild();
	}

	void SelectionModel::_unselectMolecule( const App::Component::Chemistry::Molecule & p_molecule )
	{
		_removeMolecule( p_molecule );
		_recomputeAABB();
	}

	void SelectionModel::_unselectCategory( const App::Component::Chemistry::Category & p_category )
	{
		App::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			_removeChain( *chain );
		}

		_recomputeAABB();
	}

	void SelectionModel::_unselectChain( const App::Component::Chemistry::Chain & p_chain )
	{
		_removeChain( p_chain );
		_recomputeAABB();
	}

	void SelectionModel::_unselectResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		_removeResidue( p_residue );
		_recomputeAABB();
	}

	void SelectionModel::_unselectAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		_removeAtom( p_atom );
		_recomputeAABB();
	}

	bool SelectionModel::_addMolecule( const App::Component::Chemistry::Molecule & p_molecule )
	{
		const App::Core::Model::ID & id				  = p_molecule.getId();
		const bool					 hasToAddMolecule = _items.find( id ) == _items.end();

		if ( hasToAddMolecule )
		{
			_emplaceMolecule( p_molecule );
		}

		return hasToAddMolecule;
	}

	bool SelectionModel::_addChain( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		const uint &				 index		= p_chain.getIndex();

		MapChainIds & chainMap = _moleculesMap[ moleculeId ];

		const bool hasToAddChain = chainMap.find( index ) == chainMap.end();

		if ( hasToAddChain )
		{
			chainMap.emplace( index, MapResidueIds() );
		}

		return hasToAddChain;
	}

	bool SelectionModel::_addResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
		const uint &				 chainIndex = p_residue.getChainPtr()->getIndex();
		const uint &				 index		= p_residue.getIndex();

		MapResidueIds & residueMap = _moleculesMap[ moleculeId ][ chainIndex ];

		const bool hasToAddResidue = residueMap.find( index ) == residueMap.end();

		if ( hasToAddResidue )
		{
			residueMap.emplace( index, VecAtomIds() );
		}

		return hasToAddResidue;
	}

	bool SelectionModel::_addAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		const App::Core::Model::ID & moleculeId	  = p_atom.getMoleculePtr()->getId();
		const uint &				 chainIndex	  = p_atom.getChainPtr()->getIndex();
		const uint &				 residueIndex = p_atom.getResiduePtr()->getIndex();
		const uint &				 index		  = p_atom.getIndex();

		VecAtomIds & atomVector = _moleculesMap[ moleculeId ][ chainIndex ][ residueIndex ];

		const bool hasToAddAtom = std::find( atomVector.begin(), atomVector.end(), index ) == atomVector.end();
		if ( hasToAddAtom )
		{
			atomVector.emplace_back( index );
		}

		return hasToAddAtom;
	}

	void SelectionModel::_addMoleculeContent( const App::Component::Chemistry::Molecule & p_molecule )
	{
		//_items[ p_molecule.getId() ]._setFullChildrenCount( p_molecule.getChainCount() );

		for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
		{
			const App::Component::Chemistry::Chain * const chain = p_molecule.getChain( i );

			if ( chain == nullptr )
				continue;

			_addChain( *chain );
			_addChainContent( *chain );
		}
	}

	void SelectionModel::_addChainContent( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();
		//_referenceFullChain( p_chain );

		const App::Core::Model::ID & moleculeId = moleculeParent.getId();
		const uint					 chainIndex = p_chain.getIndex();
		MapResidueIds &				 residueMap = _moleculesMap[ moleculeId ][ chainIndex ];

		if ( residueMap.getFullySelectedChildCount() == p_chain.getRealResidueCount() )
			return;

		for ( uint i = 0; i < p_chain.getResidueCount(); ++i )
		{
			const App::Component::Chemistry::Residue * const residue
				= moleculeParent.getResidue( p_chain.getIndexFirstResidue() + i );

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

	void SelectionModel::_addResidueContent( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

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
			const App::Component::Chemistry::Atom * const atom = moleculeParent.getAtom( i );

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

	void SelectionModel::_removeMolecule( const App::Component::Chemistry::Molecule & p_molecule )
	{
		const App::Core::Model::ID & id = p_molecule.getId();
		_moleculesMap.erase( id );
		_items.erase( id );
		_mapSelectionAABB.erase( id );
	}

	void SelectionModel::_removeChain( const App::Component::Chemistry::Chain & p_chain )
	{
		const App::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		const App::Core::Model::ID & index		= p_chain.getIndex();

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

	void SelectionModel::_removeResidue( const App::Component::Chemistry::Residue & p_residue )
	{
		const App::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
		const App::Core::Model::ID & chainIndex = p_residue.getChainPtr()->getIndex();
		const App::Core::Model::ID & index		= p_residue.getIndex();

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

	void SelectionModel::_removeAtom( const App::Component::Chemistry::Atom & p_atom )
	{
		const App::Core::Model::ID & moleculeId	  = p_atom.getMoleculePtr()->getId();
		const App::Core::Model::ID & chainIndex	  = p_atom.getChainPtr()->getIndex();
		const App::Core::Model::ID & residueIndex = p_atom.getResiduePtr()->getIndex();
		const App::Core::Model::ID & index		  = p_atom.getIndex();

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

	void SelectionModel::_refreshMoleculeSelection( App::Component::Chemistry::Molecule * const p_molecule )
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

	void SelectionModel::selectModels( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
									   const std::vector<App::Component::Chemistry::Category *> & p_categories,
									   const std::vector<App::Component::Chemistry::Chain *> &	  p_chains,
									   const std::vector<App::Component::Chemistry::Residue *> &  p_residues,
									   const std::vector<App::Component::Chemistry::Atom *> &	  p_atoms,
									   const bool												  p_appendToSelection,
									   const App::Core::Model::BaseModel * const				  p_currentObj )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		std::unordered_set<App::Component::Chemistry::Molecule *> moleculeSet
			= std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Component::Chemistry::Molecule * const it : p_molecules )
		{
			_selectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const App::Component::Chemistry::Category * const it : p_categories )
		{
			_selectCategory( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Chain * const it : p_chains )
		{
			_selectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Residue * const it : p_residues )
		{
			_selectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Atom * const it : p_atoms )
		{
			_selectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( App::Component::Chemistry::Molecule * const moleculePtr : moleculeSet )
		{
			_emplaceMolecule( *moleculePtr );
			moleculePtr->refreshSelection( &_moleculesMap[ moleculePtr->getId() ] );
		}

		if ( p_currentObj == nullptr )
		{
			if ( p_molecules.size() > 0 )
				_setCurrentObject( *p_molecules.crbegin() );
			else if ( p_categories.size() > 0 )
				_setCurrentObject( *p_categories.crbegin() );
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
	}
	void SelectionModel::unselectModels( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
										 const std::vector<App::Component::Chemistry::Category *> & p_categories,
										 const std::vector<App::Component::Chemistry::Chain *> &	p_chains,
										 const std::vector<App::Component::Chemistry::Residue *> &	p_residus,
										 const std::vector<App::Component::Chemistry::Atom *> &		p_atoms )
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> moleculeSet
			= std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Component::Chemistry::Molecule * const it : p_molecules )
		{
			_unselectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const App::Component::Chemistry::Category * const it : p_categories )
		{
			_unselectCategory( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Chain * const it : p_chains )
		{
			_unselectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Residue * const it : p_residus )
		{
			_unselectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Component::Chemistry::Atom * const it : p_atoms )
		{
			_unselectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( App::Component::Chemistry::Molecule * const moleculePtr : moleculeSet )
		{
			_refreshMoleculeSelection( moleculePtr );
		}

		_notifyDataChanged();
	}

	void SelectionModel::selectModel( App::Core::Model::BaseModel & p_model, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		const VTX::App::VTX_ID & modelType = p_model.getTypeId();

		if ( modelType == App::ID::Model::MODEL_MOLECULE )
			selectMolecule( static_cast<App::Component::Chemistry::Molecule &>( p_model ), p_appendToSelection );
		else if ( modelType == App::ID::Model::MODEL_CHAIN )
			selectChain( static_cast<App::Component::Chemistry::Chain &>( p_model ), p_appendToSelection );
		else if ( modelType == App::ID::Model::MODEL_RESIDUE )
			selectResidue( static_cast<App::Component::Chemistry::Residue &>( p_model ), p_appendToSelection );
		else if ( modelType == App::ID::Model::MODEL_ATOM )
			selectAtom( static_cast<App::Component::Chemistry::Atom &>( p_model ), p_appendToSelection );
		else
		{
			_items.emplace( p_model.getId() );
			_notifyDataChanged();
		}
	}

	void SelectionModel::unselectModel( const App::Core::Model::BaseModel & p_model )
	{
		_unselectModel( p_model );
		_notifyDataChanged();
	}
	void SelectionModel::_unselectModel( const App::Core::Model::BaseModel & p_model )
	{
		_items.erase( p_model.getId() );
	}

	bool SelectionModel::isCategorySelected( const App::Component::Chemistry::Category & p_category ) const
	{
		bool res = false;

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Component::Chemistry::Chain * const chain = p_category.getMoleculePtr()->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			if ( isChainSelected( *chain ) )
			{
				res = true;
				break;
			}
		}

		return res;
	}
	bool SelectionModel::isCategoryFullySelected( const App::Component::Chemistry::Category & p_category ) const
	{
		if ( p_category.isEmpty() )
			return false;

		bool res = true;

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Component::Chemistry::Chain * const chain = p_category.getMoleculePtr()->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			if ( !isChainFullySelected( *chain ) )
			{
				res = false;
				break;
			}
		}

		return res;
	}

	bool SelectionModel::isModelSelected( const App::Core::Model::ID & p_id ) const
	{
		const App::Core::Model::BaseModel & model = VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( p_id );
		return isModelSelected( model );
	}

	bool SelectionModel::isModelSelected( const App::Core::Model::BaseModel & p_model ) const
	{
		const VTX::App::VTX_ID & modelTypeID = p_model.getTypeId();

		if ( modelTypeID == App::ID::Model::MODEL_MOLECULE )
			return isMoleculeFullySelected( static_cast<const App::Component::Chemistry::Molecule &>( p_model ) );
		else if ( modelTypeID == App::ID::Model::MODEL_CHAIN )
			return isChainFullySelected( static_cast<const App::Component::Chemistry::Chain &>( p_model ) );
		else if ( modelTypeID == App::ID::Model::MODEL_RESIDUE )
			return isResidueFullySelected( static_cast<const App::Component::Chemistry::Residue &>( p_model ) );
		else if ( modelTypeID == App::ID::Model::MODEL_ATOM )
			return isAtomSelected( static_cast<const App::Component::Chemistry::Atom &>( p_model ) );
		else
			return _items.find( p_model.getId() ) != _items.end();
	}

	bool SelectionModel::isEmpty() const { return _items.size() <= 0; }

	void SelectionModel::clear()
	{
		const App::Core::Model::BaseModel * const previousCurrentObject = _currentObject;
		_clearWithoutNotify();

		_notifyDataChanged();

		if ( previousCurrentObject != _currentObject )
			VTX_EVENT<const App::Core::Model::BaseModel *>( App::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
															_currentObject );
	}
	void SelectionModel::_clearWithoutNotify()
	{
		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & item : _moleculesMap )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( item.first );
			molecule.refreshSelection( nullptr );
		}

		_items.clear();
		_moleculesMap.clear();
		_mapSelectionAABB.clear();
		_clearCurrentObject( false );
	}

	void SelectionModel::moveDataTo( SelectionModel & p_target )
	{
		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & item : _moleculesMap )
		{
			App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( item.first );
			molecule.refreshSelection( nullptr );
		}

		for ( const VTX::App::Core::Model::ID & item : _items )
			p_target._items.emplace( item );

		_items.clear();

		p_target._moleculesMap = _moleculesMap;

		// std::move( _moleculesMap.begin(), _moleculesMap.end(), p_target._moleculesMap.begin() );
		_moleculesMap.clear();

		p_target._currentObject = _currentObject;

		clear();
	}

	void SelectionModel::receiveEvent( const App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == App::Event::Global::MOLECULE_REMOVED )
		{
			const App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> & castedEvent
				= dynamic_cast<const App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> &>( p_event );
			unselectMolecule( *castedEvent.get() );
		}
	}

	void SelectionModel::_notifyDataChanged()
	{
		BaseModel::_notifyDataChanged();
		VTX_EVENT<const App::Application::Selection::SelectionModel *>( App::Event::Global::SELECTION_CHANGE, this );
	}

	void SelectionModel::getItemTypes( std::set<VTX::App::VTX_ID> & p_types ) const
	{
		p_types.clear();

		for ( const App::Core::Model::ID & id : _items )
		{
			const VTX::App::VTX_ID & typeId = VTX::MVC_MANAGER().getModelTypeID( id );

			if ( typeId == App::ID::Model::MODEL_MOLECULE )
			{
				const App::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( id );

				if ( isMoleculeFullySelected( molecule ) )
				{
					p_types.emplace( molecule.getTypeId() );
					continue;
				}

				for ( const std::pair<const App::Core::Model::ID, MapResidueIds> & chainData : _moleculesMap.at( id ) )
				{
					const App::Component::Chemistry::Chain & chain = *molecule.getChain( chainData.first );
					if ( isChainFullySelected( chain ) )
					{
						p_types.emplace( chain.getTypeId() );
						continue;
					}

					for ( const std::pair<const App::Core::Model::ID, VecAtomIds> & residueData : chainData.second )
					{
						const App::Component::Chemistry::Residue & residue = *molecule.getResidue( residueData.first );
						if ( isResidueFullySelected( residue ) )
						{
							p_types.emplace( residue.getTypeId() );
						}
						else
						{
							p_types.emplace( App::ID::Model::MODEL_ATOM );
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

	App::Component::Object3D::Helper::AABB SelectionModel::getAABB() const
	{
		App::Component::Object3D::Helper::AABB res = App::Component::Object3D::Helper::AABB();

		for ( const std::pair<const VTX::App::Core::Model::ID, App::Component::Object3D::Helper::AABB> & pairIdAabb :
			  _mapSelectionAABB )
		{
			const App::Component::Object3D::Helper::AABB & currentAABB = pairIdAabb.second;
			res.extend( currentAABB );
		}

		return res;
	}

	void SelectionModel::_recomputeAABB()
	{
		_mapSelectionAABB.clear();

		for ( const std::pair<const VTX::App::Core::Model::ID, MapChainIds> & mapMol : _moleculesMap )
		{
			const App::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( mapMol.first );
			App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB();
			_mapSelectionAABB.emplace( molecule.getId(), aabb );

			if ( molecule.getChainCount() == mapMol.second.size() )
			{
				aabb.extend( molecule.getAABB() );
				continue;
			}

			for ( const std::pair<const VTX::App::Core::Model::ID, MapResidueIds> & mapChain : mapMol.second )
			{
				const App::Component::Chemistry::Chain & chain = *molecule.getChain( mapChain.first );
				if ( chain.getResidueCount() == mapChain.second.size() )
				{
					aabb.extend( chain.getAABB() );
					continue;
				}

				for ( const std::pair<const VTX::App::Core::Model::ID, VecAtomIds> & mapResidue : mapChain.second )
				{
					const App::Component::Chemistry::Residue & residue = *molecule.getResidue( mapResidue.first );
					if ( residue.getAtomCount() == mapResidue.second.size() )
					{
						aabb.extend( residue.getAABB() );
						continue;
					}

					for ( const uint & vectorId : mapResidue.second )
					{
						const App::Component::Chemistry::Atom & atom = *molecule.getAtom( vectorId );
						aabb.extend( atom.getAABB() );
					}
				}
			}
		}
	}
	const App::Core::Model::BaseModel * const SelectionModel::getCurrentObject() const { return _currentObject; }

	void SelectionModel::_emplaceMolecule( const App::Component::Chemistry::Molecule & p_molecule )
	{
		_items.emplace( p_molecule.getId() );
		_moleculesMap.emplace( p_molecule.getId(), MapChainIds() );
	}

	void SelectionModel::_setCurrentObject( const App::Core::Model::BaseModel * const p_model, const bool p_notify )
	{
		if ( _currentObject != p_model )
		{
			_currentObject = p_model;

			if ( p_notify )
			{
				VTX_EVENT<const App::Core::Model::BaseModel *>( App::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
																_currentObject );
			}
		}
	}
	void SelectionModel::_clearCurrentObject( const bool p_notify )
	{
		if ( _currentObject != nullptr )
		{
			_currentObject = nullptr;
			if ( p_notify )
			{
				VTX_EVENT<const App::Core::Model::BaseModel *>( App::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
																_currentObject );
			}
		}
	}

} // namespace VTX::App::Application::Selection
