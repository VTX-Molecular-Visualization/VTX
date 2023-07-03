#include "app/old/action/representable.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/vtx_app.hpp"

namespace VTX::App::Old::Action::Representable
{
	void SetRepresentation::execute()
	{
		if ( _selection != nullptr )
		{
			App::Old::Application::Representation::RepresentationManager::get().instantiateRepresentations( _representation,
																									   *_selection );
		}
		else
		{
			App::Old::Application::Representation::RepresentationManager::get().instantiateRepresentation( _representation,
																									  *_representable );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
			*_representable );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Old::Action::Representable
