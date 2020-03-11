#ifndef __VTX_ACTION_MOLECULE_DELETE__
#define __VTX_ACTION_MOLECULE_DELETE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "object3d/scene.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeDelete : public BaseAction
		{
		  public:
			explicit MoleculeDelete( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

			virtual void execute() override
			{
				VTXApp::get().getScene().removeMolecule( &_molecule );
				Generic::destroy<Model::Molecule>( &_molecule );
			}

		  private:
			Model::Molecule & _molecule;
		};
	} // namespace Action
} // namespace VTX
#endif
