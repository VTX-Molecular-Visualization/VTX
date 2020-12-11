#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/residue.hpp"
#include "selection/selection_manager.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Selection
		{
			class SelectMolecule : public BaseAction
			{
			  public:
				explicit SelectMolecule( Model::Selection & p_selection, Model::Molecule & p_molecule ) : _selection( p_selection ), _molecule( p_molecule ) {}

				virtual void execute() override { _selection.selectMolecule( _molecule ); }

			  private:
				Model::Selection & _selection;
				Model::Molecule &  _molecule;
			};

			class SelectChain : public BaseAction
			{
			  public:
				explicit SelectChain( Model::Selection & p_selection, Model::Chain & p_chain ) : _selection( p_selection ), _chain( p_chain ) {}

				virtual void execute() override { _selection.selectChain( _chain ); }

			  private:
				Model::Selection & _selection;
				Model::Chain &	   _chain;
			};

			class SelectResidue : public BaseAction
			{
			  public:
				explicit SelectResidue( Model::Selection & p_selection, Model::Residue & p_residue ) : _selection( p_selection ), _residue( p_residue ) {}

				virtual void execute() override { _selection.selectResidue( _residue ); }

			  private:
				Model::Selection & _selection;
				Model::Residue &   _residue;
			};

			class SelectAtom : public BaseAction
			{
			  public:
				explicit SelectAtom( Model::Selection & p_selection, Model::Atom & p_atom ) : _selection( p_selection ), _atom( p_atom ) {}

				virtual void execute() override { _selection.selectAtom( _atom ); }

			  private:
				Model::Selection & _selection;
				Model::Atom &	   _atom;
			};

			class SelectRepresentation : public BaseAction
			{
			  public:
				explicit SelectRepresentation( Model::Selection & p_selection, Model::Representation::BaseRepresentation & p_representation ) :
					_selection( p_selection ), _representation( p_representation )
				{
				}

				virtual void execute() override { _selection.selectRepresentation( _representation ); }

			  private:
				Model::Selection &							_selection;
				Model::Representation::BaseRepresentation & _representation;
			};

			class UnselectMolecule : public BaseAction
			{
			  public:
				explicit UnselectMolecule( Model::Selection & p_selection, Model::Molecule & p_molecule ) : _selection( p_selection ), _molecule( p_molecule ) {}

				virtual void execute() override { _selection.unselectMolecule( _molecule ); }

			  private:
				Model::Selection & _selection;
				Model::Molecule &  _molecule;
			};

			class UnselectChain : public BaseAction
			{
			  public:
				explicit UnselectChain( Model::Selection & p_selection, Model::Chain & p_chain ) : _selection( p_selection ), _chain( p_chain ) {}

				virtual void execute() override { _selection.unselectChain( _chain ); }

			  private:
				Model::Selection & _selection;
				Model::Chain &	   _chain;
			};

			class UnselectResidue : public BaseAction
			{
			  public:
				explicit UnselectResidue( Model::Selection & p_selection, Model::Residue & p_residue ) : _selection( p_selection ), _residue( p_residue ) {}

				virtual void execute() override { _selection.unselectResidue( _residue ); }

			  private:
				Model::Selection & _selection;
				Model::Residue &   _residue;
			};

			class UnselectAtom : public BaseAction
			{
			  public:
				explicit UnselectAtom( Model::Selection & p_selection, Model::Atom & p_atom ) : _selection( p_selection ), _atom( p_atom ) {}

				virtual void execute() override { _selection.unselectAtom( _atom ); }

			  private:
				Model::Selection & _selection;
				Model::Atom &	   _atom;
			};

		} // namespace Selection
	}	  // namespace Action
} // namespace VTX
#endif
