#include "app/action/representable.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::App::Action::Representable
{
	void SetRepresentation::execute()
	{
		if ( _selection != nullptr )
		{
			Representation::RepresentationManager::get().instantiateRepresentations( _representation, *_selection );
		}
		else
		{
			Representation::RepresentationManager::get().instantiateRepresentation( _representation, *_representable );
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		Representation::RepresentationManager::get().removeInstantiatedRepresentation( *_representable );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Action::Representable
