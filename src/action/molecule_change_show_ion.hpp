#ifndef __VTX_ACTION_MOLECULE_CHANGE_SHOW_ION__
#define __VTX_ACTION_MOLECULE_CHANGE_SHOW_ION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeChangeShowIon : public BaseAction
		{
		  public:
			explicit MoleculeChangeShowIon( Model::Molecule & p_molecule, const bool p_showIon ) :
				_molecule( p_molecule ), _showIon( p_showIon )
			{
			}

			virtual void execute() override { _molecule.setShowIon( _showIon ); }

		  private:
			Model::Molecule & _molecule;
			const bool		  _showIon;
		};
	} // namespace Action
} // namespace VTX
#endif
