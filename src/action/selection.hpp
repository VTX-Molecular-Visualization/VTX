#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/visible.hpp"
#include "base_action.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "visible.hpp"
#include <vector>

namespace VTX::Action::Selection
{
	class SelectModels : public BaseAction
	{
	  public:
		explicit SelectModels( Model::Selection &			  p_selection,
							   const std::vector<Model::ID> & p_models,
							   const bool					  p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override
		{
			std::vector<Model::Molecule *> molecules = std::vector<Model::Molecule *>();
			std::vector<Model::Chain *>	   chains	 = std::vector<Model::Chain *>();
			std::vector<Model::Residue *>  residues	 = std::vector<Model::Residue *>();
			std::vector<Model::Atom *>	   atoms	 = std::vector<Model::Atom *>();

			for ( const Model::ID modelId : _models )
			{
				ID::VTX_ID modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );

				if ( modelTypeId == ID::Model::MODEL_MOLECULE )
				{
					Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
					molecules.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_CHAIN )
				{
					Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
					chains.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
				{
					Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
					residues.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_ATOM )
				{
					Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
					atoms.emplace_back( &model );
				}
			}

			_selection.selectModels( molecules, chains, residues, atoms, _appendToSelection );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &	   _selection;
		std::vector<Model::ID> _models = std::vector<Model::ID>();
		const bool			   _appendToSelection;
	};
	class UnselectModels : public BaseAction
	{
	  public:
		explicit UnselectModels( Model::Selection & p_selection, const std::vector<Model::ID> & p_models ) :
			_selection( p_selection )
		{
			_models.resize( p_models.size() );
			for ( int i = 0; i < _models.size(); i++ )
				_models[ i ] = p_models[ i ];
		}

		virtual void execute() override
		{
			std::vector<Model::Molecule *> molecules = std::vector<Model::Molecule *>();
			std::vector<Model::Chain *>	   chains	 = std::vector<Model::Chain *>();
			std::vector<Model::Residue *>  residues	 = std::vector<Model::Residue *>();
			std::vector<Model::Atom *>	   atoms	 = std::vector<Model::Atom *>();

			for ( const Model::ID modelId : _models )
			{
				ID::VTX_ID modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );

				if ( modelTypeId == ID::Model::MODEL_MOLECULE )
				{
					Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
					molecules.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_CHAIN )
				{
					Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
					chains.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
				{
					Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
					residues.emplace_back( &model );
				}
				else if ( modelTypeId == ID::Model::MODEL_ATOM )
				{
					Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
					atoms.emplace_back( &model );
				}
			}

			_selection.unselectModels( molecules, chains, residues, atoms );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &	   _selection;
		std::vector<Model::ID> _models = std::vector<Model::ID>();
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _selection.getAABB() );
		}

	  private:
		const Model::Selection & _selection;
	};

	class SelectMolecule : public BaseAction
	{
	  public:
		explicit SelectMolecule( Model::Selection & p_selection,
								 Model::Molecule &	p_molecule,
								 const bool			p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit SelectMolecule( Model::Selection &				  p_selection,
								 std::vector<Model::Molecule *> & p_molecules,
								 const bool						  p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override
		{
			_selection.selectMolecules( _molecules, _appendToSelection );
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Molecule *> _molecules = std::vector<Model::Molecule *>();
		const bool					   _appendToSelection;
	};

	class SelectChain : public BaseAction
	{
	  public:
		explicit SelectChain( Model::Selection & p_selection,
							  Model::Chain &	 p_chain,
							  const bool		 p_appendToSelection = false ) :
			_selection( p_selection ),
			_chain( p_chain ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override
		{
			_selection.selectChain( _chain, _appendToSelection );
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection & _selection;
		Model::Chain &	   _chain;
		const bool		   _appendToSelection;
	};

	class SelectResidue : public BaseAction
	{
	  public:
		explicit SelectResidue( Model::Selection & p_selection,
								Model::Residue &   p_residue,
								const bool		   p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit SelectResidue( Model::Selection &				p_selection,
								std::vector<Model::Residue *> & p_residues,
								const bool						p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < _residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override
		{
			_selection.selectResidues( _residues, _appendToSelection );
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &			  _selection;
		std::vector<Model::Residue *> _residues = std::vector<Model::Residue *>();
		const bool					  _appendToSelection;
	};

	class SelectAtom : public BaseAction
	{
	  public:
		explicit SelectAtom( Model::Selection & p_selection,
							 Model::Atom &		p_atom,
							 const bool			p_appendToSelection = false ) :
			_selection( p_selection ),
			_atom( p_atom ), _appendToSelection( p_appendToSelection )
		{
		}

		virtual void execute() override
		{
			_selection.selectAtom( _atom, _appendToSelection );
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection & _selection;
		Model::Atom &	   _atom;
		const bool		   _appendToSelection;
	};

	class SelectRepresentation : public BaseAction
	{
	  public:
		explicit SelectRepresentation( Model::Selection &								   p_selection,
									   Model::Representation::InstantiatedRepresentation & p_representation ) :
			_selection( p_selection ),
			_representation( p_representation )
		{
		}

		virtual void execute() override
		{
			_selection.selectRepresentation( _representation );
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &									_selection;
		Model::Representation::InstantiatedRepresentation & _representation;
	};

	class UnselectMolecule : public BaseAction
	{
	  public:
		explicit UnselectMolecule( Model::Selection & p_selection,
								   Model::Molecule &  p_molecule,
								   bool				  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.emplace_back( &p_molecule );
		}
		explicit UnselectMolecule( Model::Selection &					  p_selection,
								   const std::vector<Model::Molecule *> & p_molecules,
								   bool									  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_molecules.resize( p_molecules.size() );
			for ( int i = 0; i < _molecules.size(); i++ )
				_molecules[ i ] = p_molecules[ i ];
		}

		virtual void execute() override
		{
			if ( _check )
				_selection.unselectMolecules( _molecules );
			else
				_selection.unselectMoleculesWithCheck( _molecules );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &			   _selection;
		std::vector<Model::Molecule *> _molecules = std::vector<Model::Molecule *>();
		const bool					   _check;
	};

	class UnselectChain : public BaseAction
	{
	  public:
		explicit UnselectChain( Model::Selection & p_selection, Model::Chain & p_chain, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_chains.emplace_back( &p_chain );
		}
		explicit UnselectChain( Model::Selection &			  p_selection,
								std::vector<Model::Chain *> & p_chains,
								bool						  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_chains.resize( p_chains.size() );
			for ( int i = 0; i < p_chains.size(); i++ )
				_chains[ i ] = p_chains[ i ];
		}

		virtual void execute() override
		{
			if ( _check )
				_selection.unselectChains( _chains );
			else
				_selection.unselectChainsWithCheck( _chains );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &			_selection;
		std::vector<Model::Chain *> _chains = std::vector<Model::Chain *>();
		const bool					_check;
	};

	class UnselectResidue : public BaseAction
	{
	  public:
		explicit UnselectResidue( Model::Selection & p_selection, Model::Residue & p_residue, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_residues.emplace_back( &p_residue );
		}
		explicit UnselectResidue( Model::Selection &			  p_selection,
								  std::vector<Model::Residue *> & p_residues,
								  bool							  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_residues.resize( p_residues.size() );
			for ( int i = 0; i < p_residues.size(); i++ )
				_residues[ i ] = p_residues[ i ];
		}

		virtual void execute() override
		{
			if ( _check )
				_selection.unselectResidues( _residues );
			else
				_selection.unselectResiduesWithCheck( _residues );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &			  _selection;
		std::vector<Model::Residue *> _residues = std::vector<Model::Residue *>();
		const bool					  _check;
	};

	class UnselectAtom : public BaseAction
	{
	  public:
		explicit UnselectAtom( Model::Selection & p_selection, Model::Atom & p_atom, bool p_check = false ) :
			_selection( p_selection ), _check( p_check )
		{
			_atoms.emplace_back( &p_atom );
		}
		explicit UnselectAtom( Model::Selection &			p_selection,
							   std::vector<Model::Atom *> & p_atoms,
							   bool							p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_atoms.resize( p_atoms.size() );
			for ( int i = 0; i < p_atoms.size(); i++ )
				_atoms[ i ] = p_atoms[ i ];
		}

		virtual void execute() override
		{
			if ( _check )
				_selection.unselectAtoms( _atoms );
			else
				_selection.unselectAtomsWithCheck( _atoms );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &		   _selection;
		std::vector<Model::Atom *> _atoms = std::vector<Model::Atom *>();
		const bool				   _check;
	};

	class ClearSelection : public BaseAction
	{
	  public:
		explicit ClearSelection( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			_selection.clear();
			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	///////////////////////////// ACTION ON SELECTION ///////////////////////////////
	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( const Model::Selection &		p_selection,
								   const Generic::BaseVisible & p_objReference,
								   const ID::VTX_ID				p_objRefTypeId,
								   const VISIBILITY_MODE		p_mode ) :
			Visible::ChangeVisibility( p_mode ),
			_selection( p_selection ), _objRefTypeId( p_objRefTypeId )
		{
			_visible = _getVisibilityBool( p_objReference );
		}

		explicit ChangeVisibility( const Model::Selection & p_selection, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_mode ), _selection( p_selection ), _objRefTypeId( ID::Model::MODEL_ATOM )
		{
			_visible = _getVisibilityBool();
		}

		virtual void execute() override
		{
			const bool setVisibiltyOnMolecule = _objRefTypeId == ID::Model::MODEL_MOLECULE;
			const bool setVisibiltyOnChain	  = setVisibiltyOnMolecule || _objRefTypeId == ID::Model::MODEL_CHAIN;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getItems() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				if ( setVisibiltyOnMolecule && molIds.second.getFullySelectedChildCount() == molecule.getChainCount() )
				{
					molecule.setVisible( _visible );
				}

				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainIds : molIds.second )
				{
					Model::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getResidueCount() )
					{
						chain.setVisible( _visible );
					}

					for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueIds : chainIds.second )
					{
						Model::Residue * const residue = molecule.getResidue( residueIds.first );
						residue->setVisible( _visible );
					}
				}

				molecule.computeRepresentationTargets();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const Model::Selection & _selection;
		const ID::VTX_ID		 _objRefTypeId;
		bool					 _visible;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			std::vector<Model::Molecule *> moleculesToDelete = std::vector<Model::Molecule *>();
			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getItems() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				if ( molIds.second.getFullySelectedChildCount() == molecule.getChainCount() )
				{
					moleculesToDelete.emplace_back( &molecule );
					continue;
				}

				for ( const std::pair<uint, Model::Selection::MapResidueIds> & chainIds : molIds.second )
				{
					Model::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getResidueCount() )
					{
						molecule.removeChain( chain.getIndex(), true, true, false );
						continue;
					}

					for ( const std::pair<uint, Model::Selection::VecAtomIds> & residueIds : chainIds.second )
					{
						Model::Residue & residue = *molecule.getResidue( residueIds.first );

						if ( residueIds.second.getFullySelectedChildCount() == residue.getAtomCount() )
						{
							molecule.removeResidue( residue.getIndex(), true, true, true, false );
							continue;
						}

						for ( const uint atomId : residueIds.second )
						{
							molecule.removeAtom( atomId, true, true, true, false );
						}
					}
				}

				// Call notify only once after all modif in molecule
				molecule.forceNotifyDataChanged();
				molecule.refreshBondsBuffer();
			}

			_selection.clear();

			for ( Model::Molecule * moleculeToDelete : moleculesToDelete ) 
			{
				VTXApp::get().getScene().removeMolecule( moleculeToDelete );
				MVC::MvcManager::get().deleteModel( moleculeToDelete );
			}

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};
	////////////////////////////////////////////////////////////////////////////////////////
} // namespace VTX::Action::Selection
#endif
