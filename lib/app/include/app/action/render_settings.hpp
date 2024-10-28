#ifndef __VTX_UI_ACTION_RENDER_SETTINGS__
#define __VTX_UI_ACTION_RENDER_SETTINGS__

#include "app/action/render_settings.hpp"
#include "app/core/action/base_action.hpp"
#include <app/application/scene.hpp>
#include <app/component/representation/render_settings.hpp>

namespace VTX::App::Action::RenderSettings
{
	template<Renderer::E_RENDER_SETTINGS S, typename T>
	class ChangeRenderSetting final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeRenderSetting( const T p_value ) : _value( p_value ) {}

		void execute() override
		{
			const auto & scene	   = App::SCENE();
			auto &		 component = App::ECS_REGISTRY().getComponent<App::Component::Representation::RenderSettings>(
				  App::ECS_REGISTRY().getEntity( scene )
			  );

			component.set<S>( _value );
		}

	  private:
		const T _value;
	};

} // namespace VTX::App::Action::RenderSettings
#endif
