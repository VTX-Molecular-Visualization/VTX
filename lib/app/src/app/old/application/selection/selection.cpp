#include "app/old/application/selection/selection.hpp"
#include "app/old/component/chemistry/atom.hpp"
#include "app/old/component/chemistry/category.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/event.hpp"
#include "app/old/mvc.hpp"
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/constants.hpp>
#include <util/logger.hpp>

namespace VTX::App::Old::Application::Selection
{
	bool SelectionModel::hasItemOfType( const VTX::App::Old::VTX_ID & p_id ) const
	{
		for ( const App::Old::Core::Model::ID & modelID : getItems() )
		{
			if ( VTX::MVC_MANAGER().getModelTypeID( modelID ) == p_id )
			{
				return true;
			}
		}

		return false;
	}

	const SelectionModel::MapChainIds * const SelectionModel::getMoleculeMap(
		const App::Old::Component::Chemistry::Molecule & p_molecule )
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

	void SelectionModel::selectMolecule( App::Old::Component::Chemistry::Molecule & p_molecule,
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
	void SelectionModel::selectMolecules( const std::vector<App::Old::Component::Chemistry::Molecule *> & p_molecules,
										  const bool										p_appendToSelection,
										  const App::Old::Component::Chemistry::Molecule * const p_currentObj )
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
	void SelectionModel::selectCategory( App::Old::Component::Chemistry::Category & p_category,
										 const bool							   p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectCategory( p_category );
		_setCurrentObject( &p_category );
		_refreshMoleculeSelection( p_category.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectCategories( const std::vector<App::Old::Component::Chemistry::Category *> & p_categories,
										   const bool										 p_appendToSelection,
										   const App::Old::Component::Chemistry::Category * const p_currentObj )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();
		else if ( p_categories.size() <= 0 )
			return;

		for ( App::Old::Component::Chemistry::Category * const categoryPtr : p_categories )
			_selectCategory( *categoryPtr );

		const App::Old::Component::Chemistry::Category * const currentObj
			= p_currentObj != nullptr ? p_currentObj : *p_categories.crbegin();
		_setCurrentObject( currentObj );

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectChain( App::Old::Component::Chemistry::Chain & p_chain, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectChain( p_chain );
		_setCurrentObject( &p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectChains( const std::vector<App::Old::Component::Chemistry::Chain *> & p_chains,
									   const bool											   p_appendToSelection,
									   const App::Old::Component::Chemistry::Chain * const		   p_currentObj )
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

	void SelectionModel::selectResidue( App::Old::Component::Chemistry::Residue & p_residue, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectResidue( p_residue );
		_setCurrentObject( &p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectResidues( const std::vector<App::Old::Component::Chemistry::Residue *> & p_residues,
										 const bool												   p_appendToSelection,
										 const App::Old::Component::Chemistry::Residue * const		   p_currentObj )
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

	void SelectionModel::selectAtom( App::Old::Component::Chemistry::Atom & p_atom, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		_selectAtom( p_atom );
		_setCurrentObject( &p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::selectAtoms( const std::vector<App::Old::Component::Chemistry::Atom *> & p_atoms,
									  const bool											 p_appendToSelection,
									  const App::Old::Component::Chemistry::Atom * const			 p_currentObj )
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

	void SelectionModel::unselectMolecule( App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		_unselectMolecule( p_molecule );
		_refreshMoleculeSelection( &p_molecule );
		_notifyDataChanged();
	}
	void SelectionModel::unselectMolecules( const std::vector<App::Old::Component::Chemistry::Molecule *> & p_molecules )
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
		const std::vector<App::Old::Component::Chemistry::Molecule *> & p_molecules )
	{
		for ( const auto it : p_molecules )
		{
			App::Old::Component::Chemistry::Molecule & molecule = *it;
			if ( isMoleculeSelected( molecule ) )
			{
				_unselectMolecule( molecule );
				molecule.refreshSelection( nullptr );
			}
		}

		_notifyDataChanged();
	}

	void SelectionModel::unselectCategory( App::Old::Component::Chemistry::Category & p_category )
	{
		_unselectCategory( p_category );
		_refreshMoleculeSelection( p_category.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectCategories( const std::vector<App::Old::Component::Chemistry::Category *> & p_categories )
	{
		if ( p_categories.size() == 0 )
			return;

		for ( const App::Old::Component::Chemistry::Category * const it : p_categories )
			_unselectCategory( *it );

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectCategoriesWithCheck(
		const std::vector<App::Old::Component::Chemistry::Category *> & p_categories )
	{
		if ( p_categories.size() == 0 )
			return;

		for ( const App::Old::Component::Chemistry::Category * const it : p_categories )
		{
			if ( isCategorySelected( *it ) )
				_unselectCategory( *it );
		}

		_refreshMoleculeSelection( p_categories[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChain( App::Old::Component::Chemistry::Chain & p_chain )
	{
		_unselectChain( p_chain );
		_refreshMoleculeSelection( p_chain.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChains( const std::vector<App::Old::Component::Chemistry::Chain *> & p_chains )
	{
		if ( p_chains.size() == 0 )
			return;

		for ( const auto it : p_chains )
			_unselectChain( *it );

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectChainsWithCheck( const std::vector<App::Old::Component::Chemistry::Chain *> & p_chains )
	{
		for ( const auto it : p_chains )
		{
			if ( isChainSelected( *it ) )
				_unselectChain( *it );
		}

		_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void SelectionModel::unselectResidue( App::Old::Component::Chemistry::Residue & p_residue )
	{
		_unselectResidue( p_residue );
		_refreshMoleculeSelection( p_residue.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectResidues( const std::vector<App::Old::Component::Chemistry::Residue *> & p_residues )
	{
		if ( p_residues.size() == 0 )
			return;

		for ( const auto it : p_residues )
			_unselectResidue( *it );

		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectResiduesWithCheck(
		const std::vector<App::Old::Component::Chemistry::Residue *> & p_residues )
	{
		for ( const auto it : p_residues )
		{
			if ( isResidueSelected( *it ) )
				_unselectResidue( *it );
		}
		_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	void SelectionModel::unselectAtom( App::Old::Component::Chemistry::Atom & p_atom )
	{
		_unselectAtom( p_atom );
		_refreshMoleculeSelection( p_atom.getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectAtoms( const std::vector<App::Old::Component::Chemistry::Atom *> & p_atoms )
	{
		if ( p_atoms.size() == 0 )
			return;

		for ( const auto it : p_atoms )
			_unselectAtom( *it );

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}
	void SelectionModel::unselectAtomsWithCheck( const std::vector<App::Old::Component::Chemistry::Atom *> & p_atoms )
	{
		for ( const auto it : p_atoms )
		{
			if ( isAtomSelected( *it ) )
				_unselectAtom( *it );
		}

		_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
		_notifyDataChanged();
	}

	bool SelectionModel::isMoleculeSelected( const App::Old::Component::Chemistry::Molecule & p_molecule ) const
	{
		const App::Old::Core::Model::ID & id = p_molecule.getId();
		return _items.find( id ) != _items.end();
	}
	bool SelectionModel::isMoleculeFullySelected( const App::Old::Component::Chemistry::Molecule & p_molecule ) const
	{
		const App::Old::Core::Model::ID &		 id = p_molecule.getId();
		const MapMoleculeIds::const_iterator it = _moleculesMap.find( id );

		return _items.find( id ) != _items.end()
			   && it->second.getFullySelectedChildCount() == p_molecule.getRealChainCount();
	}
	bool SelectionModel::isChainSelected( const App::Old::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Old::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();

		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds & chainMap = _moleculesMap.at( moleculeId );
		const uint &		index	 = p_chain.getIndex();

		return chainMap.find( index ) != chainMap.end();
	}
	bool SelectionModel::isChainFullySelected( const App::Old::Component::Chemistry::Chain & p_chain ) const
	{
		const App::Old::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		if ( _items.find( moleculeId ) == _items.end() )
			return false;

		const MapChainIds &				  chainMap = _moleculesMap.at( moleculeId );
		const MapChainIds::const_iterator it	   = chainMap.find( p_chain.getIndex() );

		return it != chainMap.end() && it->second.getFullySelectedChildCount() == p_chain.getRealResidueCount();
	}
	bool SelectionModel::isResidueSelected( const App::Old::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Old::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();

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
	bool SelectionModel::isResidueFullySelected( const App::Old::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Old::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
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
	bool SelectionModel::isAtomSelected( const App::Old::Component::Chemistry::Atom & p_atom ) const
	{
		const App::Old::Core::Model::ID & moleculeId = p_atom.getMoleculePtr()->getId();

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
		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			res += (uint)mapMolecules.second.size();
		return res;
	}
	uint SelectionModel::getResidueSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapResidueIds> & mapChains : mapMolecules.second )
				res += (uint)mapChains.second.size();
		return res;
	}
	uint SelectionModel::getAtomSelectedCount() const
	{
		uint res = 0;
		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & mapMolecules : _moleculesMap )
			for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapResidueIds> & mapChains : mapMolecules.second )
				for ( const std::pair<const VTX::App::Old::Core::Model::ID, VecAtomIds> & mapResidus : mapChains.second )
					res += (uint)mapResidus.second.size();
		return res;
	}

	void SelectionModel::_selectMolecule( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		_addMolecule( p_molecule );
		_addMoleculeContent( p_molecule );

		_mapSelectionAABB[ p_molecule.getId() ].extend( p_molecule.getWorldAABB() );
	}

	void SelectionModel::_selectCategory( const App::Old::Component::Chemistry::Category & p_category )
	{
		App::Old::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( const uint chainIndex : p_category.getChains() )
		{
			App::Old::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			_selectChain( *chain );
		}
	}

	void SelectionModel::_selectChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Component::Chemistry::Molecule & parent = *p_chain.getMoleculePtr();
		_addMolecule( parent );
		_addChain( p_chain );
		_addChainContent( p_chain );

		_mapSelectionAABB[ parent.getId() ].extend( p_chain.getWorldAABB() );
	}

	void SelectionModel::_selectResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( p_residue );
		_addResidueContent( p_residue );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_residue.getWorldAABB() );
	}

	void SelectionModel::_selectAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		const App::Old::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		_addMolecule( moleculeParent );
		_addChain( chainParent );
		_addResidue( residueParent );
		const bool atomAdded = _addAtom( p_atom );

		if ( atomAdded )
			_referenceAtom( p_atom );

		_mapSelectionAABB[ moleculeParent.getId() ].extend( p_atom.getWorldAABB() );
	}

	void SelectionModel::_referenceAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		const App::Old::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		atoms._addFullChild();
		if ( atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount() )
			_referenceFullResidue( residueParent );
	}
	void SelectionModel::_referenceFullResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		residues._addFullChild();
		if ( residues.getFullySelectedChildCount() == chainParent.getRealResidueCount() )
			_referenceFullChain( chainParent );
	}
	void SelectionModel::_referenceFullChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._addFullChild();
	}

	void SelectionModel::_unreferenceAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		const App::Old::Component::Chemistry::Residue &	residueParent  = *p_atom.getResiduePtr();
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *residueParent.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		VecAtomIds & atoms
			= _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ][ residueParent.getIndex() ];
		const bool propagateToParent = atoms.getFullySelectedChildCount() == residueParent.getRealAtomCount();
		atoms._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullResidue( residueParent );
	}
	void SelectionModel::_unreferenceFullResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

		MapResidueIds & residues		  = _moleculesMap[ moleculeParent.getId() ][ chainParent.getIndex() ];
		const bool		propagateToParent = residues.getFullySelectedChildCount() == chainParent.getRealResidueCount();
		residues._removeFullChild();
		if ( propagateToParent )
			_unreferenceFullChain( chainParent );
	}
	void SelectionModel::_unreferenceFullChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();

		MapChainIds & chains = _moleculesMap[ moleculeParent.getId() ];
		chains._removeFullChild();
	}

	void SelectionModel::_unselectMolecule( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		_removeMolecule( p_molecule );
		_recomputeAABB();
	}

	void SelectionModel::_unselectCategory( const App::Old::Component::Chemistry::Category & p_category )
	{
		App::Old::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Old::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			_removeChain( *chain );
		}

		_recomputeAABB();
	}

	void SelectionModel::_unselectChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		_removeChain( p_chain );
		_recomputeAABB();
	}

	void SelectionModel::_unselectResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		_removeResidue( p_residue );
		_recomputeAABB();
	}

	void SelectionModel::_unselectAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		_removeAtom( p_atom );
		_recomputeAABB();
	}

	bool SelectionModel::_addMolecule( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		const App::Old::Core::Model::ID & id				  = p_molecule.getId();
		const bool					 hasToAddMolecule = _items.find( id ) == _items.end();

		if ( hasToAddMolecule )
		{
			_emplaceMolecule( p_molecule );
		}

		return hasToAddMolecule;
	}

	bool SelectionModel::_addChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		const uint &				 index		= p_chain.getIndex();

		MapChainIds & chainMap = _moleculesMap[ moleculeId ];

		const bool hasToAddChain = chainMap.find( index ) == chainMap.end();

		if ( hasToAddChain )
		{
			chainMap.emplace( index, MapResidueIds() );
		}

		return hasToAddChain;
	}

	bool SelectionModel::_addResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
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

	bool SelectionModel::_addAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		const App::Old::Core::Model::ID & moleculeId	  = p_atom.getMoleculePtr()->getId();
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

	void SelectionModel::_addMoleculeContent( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		//_items[ p_molecule.getId() ]._setFullChildrenCount( p_molecule.getChainCount() );

		for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
		{
			const App::Old::Component::Chemistry::Chain * const chain = p_molecule.getChain( i );

			if ( chain == nullptr )
				continue;

			_addChain( *chain );
			_addChainContent( *chain );
		}
	}

	void SelectionModel::_addChainContent( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *p_chain.getMoleculePtr();
		//_referenceFullChain( p_chain );

		const App::Old::Core::Model::ID & moleculeId = moleculeParent.getId();
		const uint					 chainIndex = p_chain.getIndex();
		MapResidueIds &				 residueMap = _moleculesMap[ moleculeId ][ chainIndex ];

		if ( residueMap.getFullySelectedChildCount() == p_chain.getRealResidueCount() )
			return;

		for ( uint i = 0; i < p_chain.getResidueCount(); ++i )
		{
			const App::Old::Component::Chemistry::Residue * const residue
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

	void SelectionModel::_addResidueContent( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Component::Chemistry::Chain &	chainParent	   = *p_residue.getChainPtr();
		const App::Old::Component::Chemistry::Molecule & moleculeParent = *chainParent.getMoleculePtr();

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
			const App::Old::Component::Chemistry::Atom * const atom = moleculeParent.getAtom( i );

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

	void SelectionModel::_removeMolecule( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		const App::Old::Core::Model::ID & id = p_molecule.getId();
		_moleculesMap.erase( id );
		_items.erase( id );
		_mapSelectionAABB.erase( id );
	}

	void SelectionModel::_removeChain( const App::Old::Component::Chemistry::Chain & p_chain )
	{
		const App::Old::Core::Model::ID & moleculeId = p_chain.getMoleculePtr()->getId();
		const App::Old::Core::Model::ID & index		= p_chain.getIndex();

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

	void SelectionModel::_removeResidue( const App::Old::Component::Chemistry::Residue & p_residue )
	{
		const App::Old::Core::Model::ID & moleculeId = p_residue.getMoleculePtr()->getId();
		const App::Old::Core::Model::ID & chainIndex = p_residue.getChainPtr()->getIndex();
		const App::Old::Core::Model::ID & index		= p_residue.getIndex();

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

	void SelectionModel::_removeAtom( const App::Old::Component::Chemistry::Atom & p_atom )
	{
		const App::Old::Core::Model::ID & moleculeId	  = p_atom.getMoleculePtr()->getId();
		const App::Old::Core::Model::ID & chainIndex	  = p_atom.getChainPtr()->getIndex();
		const App::Old::Core::Model::ID & residueIndex = p_atom.getResiduePtr()->getIndex();
		const App::Old::Core::Model::ID & index		  = p_atom.getIndex();

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

	void SelectionModel::_refreshMoleculeSelection( App::Old::Component::Chemistry::Molecule * const p_molecule )
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

	void SelectionModel::selectModels( const std::vector<App::Old::Component::Chemistry::Molecule *> & p_molecules,
									   const std::vector<App::Old::Component::Chemistry::Category *> & p_categories,
									   const std::vector<App::Old::Component::Chemistry::Chain *> &	  p_chains,
									   const std::vector<App::Old::Component::Chemistry::Residue *> &  p_residues,
									   const std::vector<App::Old::Component::Chemistry::Atom *> &	  p_atoms,
									   const bool												  p_appendToSelection,
									   const App::Old::Core::Model::BaseModel * const				  p_currentObj )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		std::unordered_set<App::Old::Component::Chemistry::Molecule *> moleculeSet
			= std::unordered_set<App::Old::Component::Chemistry::Molecule *>();

		for ( App::Old::Component::Chemistry::Molecule * const it : p_molecules )
		{
			_selectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const App::Old::Component::Chemistry::Category * const it : p_categories )
		{
			_selectCategory( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Chain * const it : p_chains )
		{
			_selectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Residue * const it : p_residues )
		{
			_selectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Atom * const it : p_atoms )
		{
			_selectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( App::Old::Component::Chemistry::Molecule * const moleculePtr : moleculeSet )
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
	void SelectionModel::unselectModels( const std::vector<App::Old::Component::Chemistry::Molecule *> & p_molecules,
										 const std::vector<App::Old::Component::Chemistry::Category *> & p_categories,
										 const std::vector<App::Old::Component::Chemistry::Chain *> &	p_chains,
										 const std::vector<App::Old::Component::Chemistry::Residue *> &	p_residus,
										 const std::vector<App::Old::Component::Chemistry::Atom *> &		p_atoms )
	{
		std::unordered_set<App::Old::Component::Chemistry::Molecule *> moleculeSet
			= std::unordered_set<App::Old::Component::Chemistry::Molecule *>();

		for ( App::Old::Component::Chemistry::Molecule * const it : p_molecules )
		{
			_unselectMolecule( *it );
			moleculeSet.emplace( it );
		}
		for ( const App::Old::Component::Chemistry::Category * const it : p_categories )
		{
			_unselectCategory( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Chain * const it : p_chains )
		{
			_unselectChain( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Residue * const it : p_residus )
		{
			_unselectResidue( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}
		for ( const App::Old::Component::Chemistry::Atom * const it : p_atoms )
		{
			_unselectAtom( *it );
			moleculeSet.emplace( it->getMoleculePtr() );
		}

		for ( App::Old::Component::Chemistry::Molecule * const moleculePtr : moleculeSet )
		{
			_refreshMoleculeSelection( moleculePtr );
		}

		_notifyDataChanged();
	}

	void SelectionModel::selectModel( App::Old::Core::Model::BaseModel & p_model, const bool p_appendToSelection )
	{
		if ( !p_appendToSelection )
			_clearWithoutNotify();

		const VTX::App::Old::VTX_ID & modelType = p_model.getTypeId();

		if ( modelType == App::Old::ID::Model::MODEL_MOLECULE )
			selectMolecule( static_cast<App::Old::Component::Chemistry::Molecule &>( p_model ), p_appendToSelection );
		else if ( modelType == App::Old::ID::Model::MODEL_CHAIN )
			selectChain( static_cast<App::Old::Component::Chemistry::Chain &>( p_model ), p_appendToSelection );
		else if ( modelType == App::Old::ID::Model::MODEL_RESIDUE )
			selectResidue( static_cast<App::Old::Component::Chemistry::Residue &>( p_model ), p_appendToSelection );
		else if ( modelType == App::Old::ID::Model::MODEL_ATOM )
			selectAtom( static_cast<App::Old::Component::Chemistry::Atom &>( p_model ), p_appendToSelection );
		else
		{
			_items.emplace( p_model.getId() );
			_notifyDataChanged();
		}
	}

	void SelectionModel::unselectModel( const App::Old::Core::Model::BaseModel & p_model )
	{
		_unselectModel( p_model );
		_notifyDataChanged();
	}
	void SelectionModel::_unselectModel( const App::Old::Core::Model::BaseModel & p_model )
	{
		_items.erase( p_model.getId() );
	}

	bool SelectionModel::isCategorySelected( const App::Old::Component::Chemistry::Category & p_category ) const
	{
		bool res = false;

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Old::Component::Chemistry::Chain * const chain = p_category.getMoleculePtr()->getChain( chainIndex );

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
	bool SelectionModel::isCategoryFullySelected( const App::Old::Component::Chemistry::Category & p_category ) const
	{
		if ( p_category.isEmpty() )
			return false;

		bool res = true;

		for ( const uint chainIndex : p_category.getChains() )
		{
			const App::Old::Component::Chemistry::Chain * const chain = p_category.getMoleculePtr()->getChain( chainIndex );

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

	bool SelectionModel::isModelSelected( const App::Old::Core::Model::ID & p_id ) const
	{
		const App::Old::Core::Model::BaseModel & model = VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( p_id );
		return isModelSelected( model );
	}

	bool SelectionModel::isModelSelected( const App::Old::Core::Model::BaseModel & p_model ) const
	{
		const VTX::App::Old::VTX_ID & modelTypeID = p_model.getTypeId();

		if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
			return isMoleculeFullySelected( static_cast<const App::Old::Component::Chemistry::Molecule &>( p_model ) );
		else if ( modelTypeID == App::Old::ID::Model::MODEL_CHAIN )
			return isChainFullySelected( static_cast<const App::Old::Component::Chemistry::Chain &>( p_model ) );
		else if ( modelTypeID == App::Old::ID::Model::MODEL_RESIDUE )
			return isResidueFullySelected( static_cast<const App::Old::Component::Chemistry::Residue &>( p_model ) );
		else if ( modelTypeID == App::Old::ID::Model::MODEL_ATOM )
			return isAtomSelected( static_cast<const App::Old::Component::Chemistry::Atom &>( p_model ) );
		else
			return _items.find( p_model.getId() ) != _items.end();
	}

	bool SelectionModel::isEmpty() const { return _items.size() <= 0; }

	void SelectionModel::clear()
	{
		const App::Old::Core::Model::BaseModel * const previousCurrentObject = _currentObject;
		_clearWithoutNotify();

		_notifyDataChanged();

		if ( previousCurrentObject != _currentObject )
			VTX_EVENT<const App::Old::Core::Model::BaseModel *>( App::Old::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
															_currentObject );
	}
	void SelectionModel::_clearWithoutNotify()
	{
		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & item : _moleculesMap )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( item.first );
			molecule.refreshSelection( nullptr );
		}

		_items.clear();
		_moleculesMap.clear();
		_mapSelectionAABB.clear();
		_clearCurrentObject( false );
	}

	void SelectionModel::moveDataTo( SelectionModel & p_target )
	{
		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & item : _moleculesMap )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( item.first );
			molecule.refreshSelection( nullptr );
		}

		for ( const VTX::App::Old::Core::Model::ID & item : _items )
			p_target._items.emplace( item );

		_items.clear();

		p_target._moleculesMap = _moleculesMap;

		// std::move( _moleculesMap.begin(), _moleculesMap.end(), p_target._moleculesMap.begin() );
		_moleculesMap.clear();

		p_target._currentObject = _currentObject;

		clear();
	}

	void SelectionModel::receiveEvent( const App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == App::Old::Event::Global::MOLECULE_REMOVED )
		{
			const App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> & castedEvent
				= dynamic_cast<const App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> &>( p_event );
			unselectMolecule( *castedEvent.get() );
		}
	}

	void SelectionModel::_notifyDataChanged()
	{
		BaseModel::_notifyDataChanged();
		VTX_EVENT<const App::Old::Application::Selection::SelectionModel *>( App::Old::Event::Global::SELECTION_CHANGE, this );
	}

	void SelectionModel::getItemTypes( std::set<VTX::App::Old::VTX_ID> & p_types ) const
	{
		p_types.clear();

		for ( const App::Old::Core::Model::ID & id : _items )
		{
			const VTX::App::Old::VTX_ID & typeId = VTX::MVC_MANAGER().getModelTypeID( id );

			if ( typeId == App::Old::ID::Model::MODEL_MOLECULE )
			{
				const App::Old::Component::Chemistry::Molecule & molecule
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( id );

				if ( isMoleculeFullySelected( molecule ) )
				{
					p_types.emplace( molecule.getTypeId() );
					continue;
				}

				for ( const std::pair<const App::Old::Core::Model::ID, MapResidueIds> & chainData : _moleculesMap.at( id ) )
				{
					const App::Old::Component::Chemistry::Chain & chain = *molecule.getChain( chainData.first );
					if ( isChainFullySelected( chain ) )
					{
						p_types.emplace( chain.getTypeId() );
						continue;
					}

					for ( const std::pair<const App::Old::Core::Model::ID, VecAtomIds> & residueData : chainData.second )
					{
						const App::Old::Component::Chemistry::Residue & residue = *molecule.getResidue( residueData.first );
						if ( isResidueFullySelected( residue ) )
						{
							p_types.emplace( residue.getTypeId() );
						}
						else
						{
							p_types.emplace( App::Old::ID::Model::MODEL_ATOM );
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

	App::Old::Component::Object3D::Helper::AABB SelectionModel::getAABB() const
	{
		App::Old::Component::Object3D::Helper::AABB res = App::Old::Component::Object3D::Helper::AABB();

		for ( const std::pair<const VTX::App::Old::Core::Model::ID, App::Old::Component::Object3D::Helper::AABB> & pairIdAabb :
			  _mapSelectionAABB )
		{
			const App::Old::Component::Object3D::Helper::AABB & currentAABB = pairIdAabb.second;
			res.extend( currentAABB );
		}

		return res;
	}

	void SelectionModel::_recomputeAABB()
	{
		_mapSelectionAABB.clear();

		for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapChainIds> & mapMol : _moleculesMap )
		{
			const App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( mapMol.first );
			App::Old::Component::Object3D::Helper::AABB aabb = App::Old::Component::Object3D::Helper::AABB();
			_mapSelectionAABB.emplace( molecule.getId(), aabb );

			if ( molecule.getChainCount() == mapMol.second.size() )
			{
				aabb.extend( molecule.getAABB() );
				continue;
			}

			for ( const std::pair<const VTX::App::Old::Core::Model::ID, MapResidueIds> & mapChain : mapMol.second )
			{
				const App::Old::Component::Chemistry::Chain & chain = *molecule.getChain( mapChain.first );
				if ( chain.getResidueCount() == mapChain.second.size() )
				{
					aabb.extend( chain.getAABB() );
					continue;
				}

				for ( const std::pair<const VTX::App::Old::Core::Model::ID, VecAtomIds> & mapResidue : mapChain.second )
				{
					const App::Old::Component::Chemistry::Residue & residue = *molecule.getResidue( mapResidue.first );
					if ( residue.getAtomCount() == mapResidue.second.size() )
					{
						aabb.extend( residue.getAABB() );
						continue;
					}

					for ( const uint & vectorId : mapResidue.second )
					{
						const App::Old::Component::Chemistry::Atom & atom = *molecule.getAtom( vectorId );
						aabb.extend( atom.getAABB() );
					}
				}
			}
		}
	}
	const App::Old::Core::Model::BaseModel * const SelectionModel::getCurrentObject() const { return _currentObject; }

	void SelectionModel::_emplaceMolecule( const App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		_items.emplace( p_molecule.getId() );
		_moleculesMap.emplace( p_molecule.getId(), MapChainIds() );
	}

	void SelectionModel::_setCurrentObject( const App::Old::Core::Model::BaseModel * const p_model, const bool p_notify )
	{
		if ( _currentObject != p_model )
		{
			_currentObject = p_model;

			if ( p_notify )
			{
				VTX_EVENT<const App::Old::Core::Model::BaseModel *>( App::Old::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
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
				VTX_EVENT<const App::Old::Core::Model::BaseModel *>( App::Old::Event::Global::CURRENT_ITEM_IN_SELECTION_CHANGE,
																_currentObject );
			}
		}
	}

} // namespace VTX::App::Old::Application::Selection
