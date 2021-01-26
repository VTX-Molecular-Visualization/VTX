#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "visible.hpp"
#include <vector>

namespace VTX
{
	namespace Action
	{
		namespace Selection
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
					if ( !_appendToSelection )
						_selection.clear();

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

					if ( molecules.size() > 0 )
						_selection.selectMolecules( molecules, true );
					if ( chains.size() > 0 )
						_selection.selectChains( chains, true );
					if ( residues.size() > 0 )
						_selection.selectResidues( residues, true );
					if ( atoms.size() > 0 )
						_selection.selectAtoms( atoms, true );

					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection &	   _selection;
				std::vector<Model::ID> _models = std::vector<Model::ID>();
				const bool			   _appendToSelection;
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
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
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
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
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
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
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
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
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
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection &									_selection;
				Model::Representation::InstantiatedRepresentation & _representation;
			};

			class UnselectMolecule : public BaseAction
			{
			  public:
				explicit UnselectMolecule( Model::Selection & p_selection, Model::Molecule & p_molecule ) :
					_selection( p_selection ), _molecule( p_molecule )
				{
				}

				virtual void execute() override
				{
					_selection.unselectMolecule( _molecule );
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection & _selection;
				Model::Molecule &  _molecule;
			};

			class UnselectChain : public BaseAction
			{
			  public:
				explicit UnselectChain( Model::Selection & p_selection, Model::Chain & p_chain ) :
					_selection( p_selection ), _chain( p_chain )
				{
				}

				virtual void execute() override
				{
					_selection.unselectChain( _chain );
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection & _selection;
				Model::Chain &	   _chain;
			};

			class UnselectResidue : public BaseAction
			{
			  public:
				explicit UnselectResidue( Model::Selection & p_selection, Model::Residue & p_residue ) :
					_selection( p_selection ), _residue( p_residue )
				{
				}

				virtual void execute() override
				{
					_selection.unselectResidue( _residue );
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection & _selection;
				Model::Residue &   _residue;
			};

			class UnselectAtom : public BaseAction
			{
			  public:
				explicit UnselectAtom( Model::Selection & p_selection, Model::Atom & p_atom ) :
					_selection( p_selection ), _atom( p_atom )
				{
				}

				virtual void execute() override
				{
					_selection.unselectAtom( _atom );
					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				Model::Selection & _selection;
				Model::Atom &	   _atom;
			};

			///////////////////////////// ACTION ON SELECTION ///////////////////////////////
			class ChangeVisibility : public Visible::ChangeVisibility
			{
			  public:
				explicit ChangeVisibility( const Model::Selection &		p_selection,
										   const Generic::BaseVisible & p_objReference,
										   const VISIBILITY_MODE		p_mode ) :
					Visible::ChangeVisibility( p_mode ),
					_selection( p_selection ), _objReference( p_objReference )
				{
				}

				virtual void execute() override
				{
					bool visible	   = _getVisibilityBool( _objReference );
					bool applyToParent = true;

					for ( const std::pair<Model::ID, Model::Selection::MapChainIds> molIds : _selection.getItems() )
					{
						Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molIds.first );
						applyToParent			   = true;

						for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> chainIds : molIds.second )
						{
							Model::Chain & chain = *molecule.getChain( chainIds.first );
							for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> residueIds :
								  chainIds.second )
							{
								Model::Residue * const residue = molecule.getResidue( residueIds.first );
								residue->setVisible( visible );
							}

							if ( chain.getResidueCount() == chainIds.second.size() )
							{
								chain.setVisible( visible );
							}
							else
							{
								applyToParent = false;
							}
						}

						if ( applyToParent && molecule.getChainCount() == molIds.second.size() )
						{
							molecule.setVisible( visible );
						}
						else
						{
							applyToParent = false;
						}

						molecule.computeRepresentationTargets();
					}

					VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
				}

			  private:
				const Model::Selection &	 _selection;
				const Generic::BaseVisible & _objReference;
			};
			////////////////////////////////////////////////////////////////////////////////////////
		} // namespace Selection
	}	  // namespace Action
} // namespace VTX
#endif
