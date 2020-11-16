#ifndef __VTX_ACTION_SELECTION__
#define __VTX_ACTION_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "selection/selection_manager.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Selection
		{
			class UpdateSelection : public BaseAction
			{
			  public:
				explicit UpdateSelection( Model::Selection & p_selection, const std::vector<Model::ID> & p_selectedIds ) : _selection( p_selection ), _selectedIds( p_selectedIds )
				{
					_selection.update( _selectedIds );
				}

				virtual void execute() override {}

			  private:
				Model::Selection &	   _selection;
				std::vector<Model::ID> _selectedIds;
			};

			class SelectMolecule : public BaseAction
			{
			  public:
				explicit SelectMolecule( Model::Selection & p_selection, const Model::Molecule & p_molecule ) : _selection( p_selection ), _molecule( p_molecule ) {}

				virtual void execute() override { _selection.selectMolecule( _molecule ); }

			  private:
				Model::Selection &		_selection;
				const Model::Molecule & _molecule;
			};

			class SelectChain : public BaseAction
			{
			  public:
				explicit SelectChain( Model::Selection & p_selection, const Model::Chain & p_chain ) : _selection( p_selection ), _chain( p_chain ) {}

				virtual void execute() override { _selection.selectChain( _chain ); }

			  private:
				Model::Selection &	 _selection;
				const Model::Chain & _chain;
			};

			class SelectResidue : public BaseAction
			{
			  public:
				explicit SelectResidue( Model::Selection & p_selection, const Model::Residue & p_residue ) : _selection( p_selection ), _residue( p_residue ) {}

				virtual void execute() override { _selection.selectResidue( _residue ); }

			  private:
				Model::Selection &	   _selection;
				const Model::Residue & _residue;
			};

			class SelectAtom : public BaseAction
			{
			  public:
				explicit SelectAtom( Model::Selection & p_selection, const Model::Atom & p_atom ) : _selection( p_selection ), _atom( p_atom ) {}

				virtual void execute() override { _selection.selectAtom( _atom ); }

			  private:
				Model::Selection &	_selection;
				const Model::Atom & _atom;
			};

			class UnselectMolecule : public BaseAction
			{
			  public:
				explicit UnselectMolecule( Model::Selection & p_selection, const Model::Molecule & p_molecule ) : _selection( p_selection ), _molecule( p_molecule ) {}

				virtual void execute() override { _selection.unselectMolecule( _molecule ); }

			  private:
				Model::Selection &		_selection;
				const Model::Molecule & _molecule;
			};

			class UnselectChain : public BaseAction
			{
			  public:
				explicit UnselectChain( Model::Selection & p_selection, const Model::Chain & p_chain ) : _selection( p_selection ), _chain( p_chain ) {}

				virtual void execute() override { _selection.unselectChain( _chain ); }

			  private:
				Model::Selection &	 _selection;
				const Model::Chain & _chain;
			};

			class UnselectResidue : public BaseAction
			{
			  public:
				explicit UnselectResidue( Model::Selection & p_selection, const Model::Residue & p_residue ) : _selection( p_selection ), _residue( p_residue ) {}

				virtual void execute() override { _selection.unselectResidue( _residue ); }

			  private:
				Model::Selection &	   _selection;
				const Model::Residue & _residue;
			};

			class UnselectAtom : public BaseAction
			{
			  public:
				explicit UnselectAtom( Model::Selection & p_selection, const Model::Atom & p_atom ) : _selection( p_selection ), _atom( p_atom ) {}

				virtual void execute() override { _selection.unselectAtom( _atom ); }

			  private:
				Model::Selection &	_selection;
				const Model::Atom & _atom;
			};

		} // namespace Selection
	}	  // namespace Action
} // namespace VTX
#endif
