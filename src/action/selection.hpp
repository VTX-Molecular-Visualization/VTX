#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#include "action/visible.hpp"
#include "base_action.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/chrono.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"
#include <vector>

namespace VTX::Action::Selection
{
	class SelectAll : public BaseAction
	{
	  public:
		explicit SelectAll() {}

		virtual void execute() override
		{
			const Object3D::Scene::MapMoleculePtrFloat & sceneMolecules = VTXApp::get().getScene().getMolecules();
			std::vector<Model::Molecule *>				 molecules		= std::vector<Model::Molecule *>();
			molecules.resize( sceneMolecules.size() );
			int counter = 0;

			for ( const std::pair<Model::Molecule *, float> & sceneMolecule : sceneMolecules )
			{
				molecules[ counter ] = sceneMolecule.first;
				counter++;
			}

			VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecules( molecules );

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}
	};
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

			std::vector<Model::Path *>		paths	   = std::vector<Model::Path *>();
			std::vector<Model::Viewpoint *> viewpoints = std::vector<Model::Viewpoint *>();

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
				else if ( modelTypeId == ID::Model::MODEL_PATH )
				{
					Model::Path & path = MVC::MvcManager::get().getModel<Model::Path>( modelId );

					for ( Model::Viewpoint * const viewpoint : path.getViewpoints() )
						viewpoints.emplace_back( viewpoint );
				}
				else if ( modelTypeId == ID::Model::MODEL_VIEWPOINT )
				{
					Model::Viewpoint & model = MVC::MvcManager::get().getModel<Model::Viewpoint>( modelId );
					viewpoints.emplace_back( &model );
				}
			}

			if ( !_appendToSelection )
				_selection.clear();

			_selection.selectModels( molecules, chains, residues, atoms, true );
			_selection.selectModels( paths, true );
			_selection.selectModels( viewpoints, true );

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

	class SelectViewpoint : public BaseAction
	{
	  public:
		explicit SelectViewpoint( Model::Selection & p_selection,
								  Model::Viewpoint & p_viewpoint,
								  const bool		 p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit SelectViewpoint( Model::Selection &				p_selection,
								  std::vector<Model::Viewpoint *> & p_viewpoints,
								  const bool						p_appendToSelection = false ) :
			_selection( p_selection ),
			_appendToSelection( p_appendToSelection )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < _viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];

			// VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

		virtual void execute() override { _selection.selectModels( _viewpoints, _appendToSelection ); }

	  private:
		Model::Selection &				_selection;
		std::vector<Model::Viewpoint *> _viewpoints = std::vector<Model::Viewpoint *>();
		const bool						_appendToSelection;
	};
	class UnselectViewpoint : public BaseAction
	{
	  public:
		explicit UnselectViewpoint( Model::Selection & p_selection,
									Model::Viewpoint & p_viewpoint,
									bool			   p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.emplace_back( &p_viewpoint );
		}
		explicit UnselectViewpoint( Model::Selection &				  p_selection,
									std::vector<Model::Viewpoint *> & p_viewpoints,
									bool							  p_check = false ) :
			_selection( p_selection ),
			_check( p_check )
		{
			_viewpoints.resize( p_viewpoints.size() );
			for ( int i = 0; i < p_viewpoints.size(); i++ )
				_viewpoints[ i ] = p_viewpoints[ i ];
		}

		virtual void execute() override
		{
			if ( _check )
				_selection.unselectModels( _viewpoints );
			else
				_selection.unselectModelsWithCheck( _viewpoints );

			// VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		}

	  private:
		Model::Selection &				_selection;
		std::vector<Model::Viewpoint *> _viewpoints = std::vector<Model::Viewpoint *>();
		const bool						_check;
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
								   const ID::VTX_ID &			p_objRefTypeId,
								   const VISIBILITY_MODE		p_mode ) :
			Visible::ChangeVisibility( p_mode ),
			_selection( p_selection ), _objRefTypeId( p_objRefTypeId )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		explicit ChangeVisibility( const Model::Selection & p_selection, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_mode ), _selection( p_selection ), _objRefTypeId( "" )
		{
		}

		virtual void execute() override
		{
			switch ( _mode )
			{
			case VISIBILITY_MODE::SHOW:
			case VISIBILITY_MODE::ALL: show(); break;
			case VISIBILITY_MODE::HIDE: hide(); break;
			case VISIBILITY_MODE::SOLO: solo(); break;
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		void show()
		{
			const bool setVisibiltyOnMolecule = _objRefTypeId == ID::Model::MODEL_MOLECULE;
			const bool setVisibiltyOnChain	  = setVisibiltyOnMolecule || _objRefTypeId == ID::Model::MODEL_CHAIN;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				molecule.setVisible( true );

				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainIds : molIds.second )
				{
					Model::Chain & chain = *molecule.getChain( chainIds.first );

					if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						chain.setVisible( true );
					}

					for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueIds : chainIds.second )
					{
						Model::Residue & residue = *molecule.getResidue( residueIds.first );

						if ( residueIds.second.getFullySelectedChildCount() == residue.getRealAtomCount() )
						{
							residue.setVisible( true );
						}

						for ( const uint atomId : residueIds.second )
						{
							Model::Atom * const atom = molecule.getAtom( atomId );
							atom->setVisible( true );
						}
					}
				}

				molecule.refreshVisibilities();
				molecule.computeRepresentationTargets();
			}
		}
		void hide()
		{
			const bool setVisibiltyOnMolecule = _objRefTypeId == ID::Model::MODEL_MOLECULE;
			const bool setVisibiltyOnChain	  = setVisibiltyOnMolecule || _objRefTypeId == ID::Model::MODEL_CHAIN;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				if ( setVisibiltyOnMolecule
					 && molIds.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
				{
					molecule.setVisible( false );
				}
				else
				{
					for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainIds : molIds.second )
					{
						Model::Chain & chain = *molecule.getChain( chainIds.first );

						if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
						{
							chain.setVisible( false );
							continue;
						}

						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueIds : chainIds.second )
						{
							Model::Residue & residue = *molecule.getResidue( residueIds.first );

							if ( residueIds.second.getFullySelectedChildCount() == residue.getRealAtomCount() )
							{
								residue.setVisible( false );
								continue;
							}

							for ( const uint atomId : residueIds.second )
							{
								Model::Atom * const atom = molecule.getAtom( atomId );
								atom->setVisible( false );
							}
						}
					}
				}

				molecule.refreshVisibilities();
				molecule.computeRepresentationTargets();
			}
		}
		void solo()
		{
			const Object3D::Scene::MapMoleculePtrFloat & moleculesInScene	  = VTXApp::get().getScene().getMolecules();
			const Model::Selection::MapMoleculeIds &	 moleculesInSelection = _selection.getMoleculesMap();

			Object3D::Scene::MapMoleculePtrFloat::const_iterator itScene	 = moleculesInScene.cbegin();
			Model::Selection::MapMoleculeIds::const_iterator	 itSelection = moleculesInSelection.cbegin();

			while ( itSelection != moleculesInSelection.cend() )
			{
				if ( itScene->first->getId() == itSelection->first )
				{
					Model::Molecule * const molecule = itScene->first;
					molecule->setVisible( true );

					// if ( !_selection.isMoleculeFullySelected( *molecule ) )
					{
						Model::Selection::MapChainIds::const_iterator itChainSelection = itSelection->second.cbegin();

						for ( uint iChain = 0; iChain < molecule->getChainCount(); iChain++ )
						{
							Model::Chain * const chain = molecule->getChain( iChain );

							if ( chain == nullptr )
								continue;

							if ( itChainSelection != itSelection->second.cend() && iChain == itChainSelection->first )
							{
								chain->setVisible( true );

								// if ( !_selection.isChainFullySelected( *chain ) )
								{
									Model::Selection::MapResidueIds::const_iterator itResidueSelection
										= itChainSelection->second.cbegin();

									for ( uint iResidue = chain->getIndexFirstResidue();
										  iResidue <= chain->getIndexLastResidue();
										  iResidue++ )
									{
										Model::Residue * const residue = molecule->getResidue( iResidue );

										if ( residue == nullptr )
											continue;

										if ( itResidueSelection != itChainSelection->second.cend()
											 && iResidue == itResidueSelection->first )
										{
											residue->setVisible( true );

											// if ( !_selection.isResidueFullySelected( *residue ) )
											{
												Model::Selection::VecAtomIds::const_iterator itAtomSelection
													= itResidueSelection->second.cbegin();

												for ( uint iAtom = residue->getIndexFirstAtom();
													  iAtom < residue->getIndexFirstAtom() + residue->getAtomCount();
													  iAtom++ )
												{
													Model::Atom * const atom = molecule->getAtom( iAtom );

													if ( atom == nullptr )
														continue;

													if ( itAtomSelection != itResidueSelection->second.cend()
														 && iAtom == *itAtomSelection )
													{
														atom->setVisible( true );
														itAtomSelection++;
													}
													else
													{
														atom->setVisible( false );
													}
												}
											}

											itResidueSelection++;
										}
										else
										{
											residue->setVisible( false );
										}
									}
								}
								itChainSelection++;
							}
							else
							{
								chain->setVisible( false );
							}
						}
					}

					molecule->refreshVisibilities();
					molecule->computeRepresentationTargets();

					itSelection++;
					itScene++;
				}
				else
				{
					itScene->first->setVisible( false );
					itScene->first->refreshVisibilities();
					itScene->first->computeRepresentationTargets();

					itScene++;
				}
			}
		}

	  private:
		const Model::Selection & _selection;
		const ID::VTX_ID		 _objRefTypeId;
	};

	class ToggleWatersVisibility : public BaseAction
	{
	  public:
		explicit ToggleWatersVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			bool showWater = true;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				showWater				   = showWater && !molecule.showWater();
			}

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				molecule.setShowWater( showWater );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	class ToggleSolventVisibility : public BaseAction
	{
	  public:
		explicit ToggleSolventVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			bool showSolvent = true;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				showSolvent				   = showSolvent && !molecule.showSolvent();
			}

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				molecule.setShowSolvent( showSolvent );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	class ToggleHydrogensVisibility : public BaseAction
	{
	  public:
		explicit ToggleHydrogensVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			bool showHydrogen = true;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				showHydrogen			   = showHydrogen && !molecule.showHydrogen();
			}

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				molecule.setShowHydrogen( showHydrogen );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	class ToggleIonsVisibility : public BaseAction
	{
	  public:
		explicit ToggleIonsVisibility( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			bool showIons = true;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				showIons				   = showIons && !molecule.showIon();
			}

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				molecule.setShowIon( showIons );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	class ToggleTrajectoryPlaying : public BaseAction
	{
	  public:
		explicit ToggleTrajectoryPlaying( Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			bool play = true;

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
				if ( molecule.hasTrajectory() )
					play = play && !molecule.isPlaying();
			}

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds : _selection.getMoleculesMap() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

				if ( molecule.hasTrajectory() )
				{
					if ( molecule.isAtEndOfTrajectoryPlay() && play )
						molecule.resetTrajectoryPlay();

					molecule.setIsPlaying( play );
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Selection & p_selection, const int p_indexPreset ) :
			_selection( p_selection ), _indexPreset( p_indexPreset )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Model::Representation::Representation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			Representation::RepresentationManager::get().instantiateRepresentations( preset, _selection );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Selection & _selection;
		const int		   _indexPreset;
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Selection & p_selection ) : _selection( p_selection ) {}

		virtual void execute() override
		{
			const Math::AABB target
				= _selection.hasMolecule() ? _selection.getAABB() : VTXApp::get().getScene().getAABB();

			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( target );
		}

	  private:
		const Model::Selection & _selection;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Selection & p_source ) : _selection( p_source )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			Tool::Chrono chrono;

			for ( const Model::ID & selectedObjectID : _selection.getItems() )
			{
				const ID::VTX_ID & modelTypeID = MVC::MvcManager::get().getModelTypeID( selectedObjectID );

				if ( modelTypeID == ID::Model::MODEL_MOLECULE )
				{
					Model::GeneratedMolecule * generatedMolecule
						= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

					generatedMolecule->copyFromSelection( _selection, selectedObjectID );

					chrono.start();
					VTX_EVENT(
						new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

					const float offset = generatedMolecule->getAABB().radius() + _selection.getAABB().radius()
										 + VTX::Setting::COPIED_MOLECULE_OFFSET;

					generatedMolecule->setTranslation( VTX::Vec3f( offset, 0, 0 ) );
					VTXApp::get().getScene().addMolecule( generatedMolecule );
					chrono.stop();
					VTX_DEBUG( "Molecule " + generatedMolecule->getDisplayName() + " copied in "
							   + chrono.elapsedTimeStr() );
				}
			}
		}

	  private:
		const Model::Selection & _selection;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Selection & p_source ) : _selection( p_source )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & moleculeSelectionData :
				  _selection.getMoleculesMap() )
			{
				const Model::ID & idMolSource = moleculeSelectionData.first;
				Model::Molecule & molecule	  = MVC::MvcManager::get().getModel<Model::Molecule>( idMolSource );

				if ( _selection.isMoleculeFullySelected( molecule ) )
					continue;

				Model::GeneratedMolecule * const generatedMolecule
					= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

				generatedMolecule->extractFromSelection( _selection, idMolSource );

				VTX_EVENT(
					new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

				VTXApp::get().getScene().addMolecule( generatedMolecule );
			}

			VTX::Selection::SelectionManager::get().getSelectionModel().clear();

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const Model::Selection & _selection;
	};
	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Selection & p_selection ) : _selection( p_selection )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			std::vector<Model::Molecule *> moleculesToDelete = std::vector<Model::Molecule *>();

			const std::set<Model::ID>		 itemsToDeleteCopy		 = _selection.getItems();
			Model::Selection::MapMoleculeIds moleculeMapToDeleteCopy = _selection.getMoleculesMap();
			_selection.clear();

			for ( const Model::ID & selectedObjectID : itemsToDeleteCopy )
			{
				const ID::VTX_ID & modelTypeID = MVC::MvcManager::get().getModelTypeID( selectedObjectID );

				if ( modelTypeID == ID::Model::MODEL_MOLECULE )
				{
					const std::pair<Model::ID, Model::Selection::MapChainIds> & molIds
						= *moleculeMapToDeleteCopy.find( selectedObjectID );

					Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );

					if ( molIds.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
					{
						moleculesToDelete.emplace_back( &molecule );
						continue;
					}

					for ( const std::pair<uint, Model::Selection::MapResidueIds> & chainIds : molIds.second )
					{
						Model::Chain & chain = *molecule.getChain( chainIds.first );

						if ( chainIds.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
						{
							molecule.removeChain( chain.getIndex(), true, true, false );
							continue;
						}

						for ( const std::pair<uint, Model::Selection::VecAtomIds> & residueIds : chainIds.second )
						{
							Model::Residue & residue = *molecule.getResidue( residueIds.first );

							if ( residueIds.second.getFullySelectedChildCount() == residue.getRealAtomCount() )
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

					if ( molecule.isEmpty() )
					{
						moleculesToDelete.emplace_back( &molecule );
					}
					else
					{
						// Call notify only once after all modif in molecule
						molecule.refreshStructure();
						molecule.computeAllRepresentationData();
						molecule.notifyStructureChange();
					}
				}
				else if ( modelTypeID == ID::Model::MODEL_PATH )
				{
					Model::Path & path = MVC::MvcManager::get().getModel<Model::Path>( selectedObjectID );

					VTX::VTXApp::get().getScene().removePath( &path );
					MVC::MvcManager::get().deleteModel( &path );
				}
				else if ( modelTypeID == ID::Model::MODEL_VIEWPOINT )
				{
					Model::Viewpoint & viewpoint
						= MVC::MvcManager::get().getModel<Model::Viewpoint>( selectedObjectID );

					Model::Path * const path = viewpoint.getPathPtr();
					path->removeViewpoint( &viewpoint );
					MVC::MvcManager::get().deleteModel( &viewpoint );

					path->refreshAllDurations();
				}
			}

			for ( Model::Molecule * const moleculeToDelete : moleculesToDelete )
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
