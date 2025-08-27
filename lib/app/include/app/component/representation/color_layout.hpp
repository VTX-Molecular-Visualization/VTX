#ifndef __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__
#define __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__

#include "app/core/ecs/base_component.hpp"
#include "app/library/preset/color.hpp"
#include <core/struct/color_layout.hpp>
#include <renderer/proxy/color_layout.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class ColorLayout :
		public Core::ECS::BaseComponentProxyPreset<Renderer::Proxy::ColorLayout, App::Library::Preset::Color>
	{
	  public:
		ColorLayout() = delete;
		ColorLayout( Library::Preset::Color & p_preset );

		void setupProxy() override;
	};

} // namespace VTX::App::Component::Representation

#endif
