#ifndef __VTX_ACTION_CHANGE_REPRESENTATION__
#define __VTX_ACTION_CHANGE_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeRepresentation : public BaseAction
		{
		  public:
			ActionChangeRepresentation( View::MOLECULE_REPRESENTATION p_representation ) :
				_representation( p_representation )
			{
				_representationOld = Setting::Rendering::representation;
			}

			virtual bool canBeUndone() override { return true; }

			virtual void execute() override
			{
				Setting::Rendering::representation = _representation;
				for ( Model::ModelMolecule * const molecule : VTXApp::get().getScene().getMolecules() )
				{
					molecule->setRepresentation();
				}
			};

			virtual void undo() override
			{
				Setting::Rendering::representation = _representationOld;
				for ( Model::ModelMolecule * const molecule : VTXApp::get().getScene().getMolecules() )
				{
					molecule->setRepresentation();
				}
			}

		  private:
			View::MOLECULE_REPRESENTATION _representation;
			View::MOLECULE_REPRESENTATION _representationOld;
		};
	} // namespace Action
} // namespace VTX
#endif
