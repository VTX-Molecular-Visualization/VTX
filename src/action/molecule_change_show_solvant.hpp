#ifndef __VTX_ACTION_MOLECULE_CHANGE_SHOW_SOLVANT__
#define __VTX_ACTION_MOLECULE_CHANGE_SHOW_SOLVANT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeShowSolvant : public BaseAction
		{
		  public:
			explicit MoleculeChangeShowSolvant( Model::Molecule & p_molecule, const bool p_showSolvant ) :
				_molecule( p_molecule ), _showSolvant( p_showSolvant )
			{
			}

			virtual void execute() override { _molecule.setShowSolvant( _showSolvant ); }

		  private:
			Model::Molecule & _molecule;
			const bool		  _showSolvant;
		};
	} // namespace Action
} // namespace VTX
#endif
