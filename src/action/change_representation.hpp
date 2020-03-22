#ifndef __VTX_ACTION_CHANGE_REPRESENTATION__
#define __VTX_ACTION_CHANGE_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeRepresentation : public BaseAction
		{
		  public:
			explicit ChangeRepresentation( const View::MOLECULE_REPRESENTATION p_representation ) :
				_representation( p_representation )
			{
			}

			virtual void execute() override
			{
				Setting::Rendering::representation = _representation;
				for ( const Object3D::Scene::PairMoleculePtrFloat pair : VTXApp::get().getScene().getMolecules() )
				{
					pair.first->setRepresentation();
				}
			};

		  private:
			const View::MOLECULE_REPRESENTATION _representation;
		};
	} // namespace Action
} // namespace VTX
#endif
