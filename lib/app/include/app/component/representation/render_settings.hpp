#ifndef __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__
#define __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__

#include "app/core/ecs/base_component.hpp"
#include "app/library/preset/render_settings.hpp"
#include <renderer/proxy/render_settings.hpp>

namespace VTX::App::Component::Representation
{

	class RenderSettings :
		public Core::ECS::
			BaseComponentProxyPreset<Renderer::Proxy::RenderSettings, App::Library::Preset::RenderSettings>
	{
	  public:
		RenderSettings() = delete;
		RenderSettings( Library::Preset::RenderSettings & p_preset );

		void setupProxy() override;
	};

} // namespace VTX::App::Component::Representation

#endif
