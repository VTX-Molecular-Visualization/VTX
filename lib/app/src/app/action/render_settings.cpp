#include "app/action/render_settings.hpp"
#include <app/application/scene.hpp>
#include <app/component/representation/render_settings.hpp>

namespace VTX::App::Action::RenderSettings
{

	// ChangeRenderSetting::ChangeRenderSetting() {}

	/*
	void ChangeRenderSetting::execute()
	{
		const auto & scene	   = App::SCENE();
		auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::RenderSettings>(
			  App::ECS_REGISTRY().getEntity( scene )
		  );

		// component.set( _index, _color );
	}
	*/
} // namespace VTX::App::Action::RenderSettings
