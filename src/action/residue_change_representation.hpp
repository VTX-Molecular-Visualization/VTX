#ifndef __VTX_ACTION_RESIDUE_CHANGE_REPRESENTATION__
#define __VTX_ACTION_RESIDUE_CHANGE_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/residue.hpp"
#include "util/molecule.hpp"

namespace VTX
{
	namespace Action
	{
		class ResidueChangeRepresentation : public BaseAction
		{
		  public:
			explicit ResidueChangeRepresentation( Model::Residue &				p_residue,
												  const Generic::REPRESENTATION p_representation ) :
				_residue( p_residue ),
				_representation( p_representation )
			{
			}

			virtual void execute() override { _residue.setRepresentation( _representation ); }

		  private:
			Model::Residue &			  _residue;
			const Generic::REPRESENTATION _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
