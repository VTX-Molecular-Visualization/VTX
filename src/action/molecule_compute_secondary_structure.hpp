#ifndef __VTX_ACTION_MOLECULE_COMPUTE_SECONDARY_STRUCTURE__
#define __VTX_ACTION_MOLECULE_COMPUTE_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "util/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class MoleculeComputeSecondaryStructure : public BaseAction
		{
		  public:
			explicit MoleculeComputeSecondaryStructure( Model::Molecule & p_molecule ) : _molecule( p_molecule ) {}

			virtual void execute() override
			{
				Util::Molecule::computeSecondaryStructure( _molecule );
				_molecule.createSecondaryStructure();
			}

		  private:
			Model::Molecule & _molecule;
		};
	} // namespace Action
} // namespace VTX
#endif
