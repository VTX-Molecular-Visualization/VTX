#ifndef __VTX_ACTION_MOLECULE_CHANGE_SHOW_Solvent__
#define __VTX_ACTION_MOLECULE_CHANGE_SHOW_Solvent__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeShowSolvent : public BaseAction
		{
		  public:
			explicit MoleculeChangeShowSolvent( Model::Molecule & p_molecule, const bool p_showSolvent ) :
				_molecule( p_molecule ), _showSolvent( p_showSolvent )
			{
			}

			virtual void execute() override { _molecule.setShowSolvent( _showSolvent ); }

		  private:
			Model::Molecule & _molecule;
			const bool		  _showSolvent;
		};
	} // namespace Action
} // namespace VTX
#endif
