#ifndef __VTX_ACTION_MOLECULE_CHANGE_REPRESENTATION__
#define __VTX_ACTION_MOLECULE_CHANGE_REPRESENTATION__

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
		class MoleculeChangeRepresentation : public BaseAction
		{
		  public:
			explicit MoleculeChangeRepresentation( Model::Molecule &			 p_molecule,
												   const Generic::REPRESENTATION p_representation ) :
				_molecule( p_molecule ),
				_representation( p_representation )
			{
			}

			virtual void execute() override { _molecule.setRepresentation( _representation ); };

		  private:
			Model::Molecule &			  _molecule;
			const Generic::REPRESENTATION _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
