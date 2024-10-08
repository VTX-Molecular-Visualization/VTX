#ifndef __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__
#define __VTX_APP_COMPONENT_REPRESENTATION_COLOR_LAYOUT__

#include "app/core/ecs/base_component.hpp"
#include <core/struct/color_layout.hpp>
#include <renderer/proxy/color_layout.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class ColorLayout : public Core::ECS::BaseComponentProxy<Renderer::Proxy::ColorLayout>
	{
	  public:
		ColorLayout();

		void setupProxy() override;

		const VTX::Core::Struct::ColorLayout & getLayout() const { return _layout; }

		void setColor( const size_t p_index, const Util::Color::Rgba & p_color );
		void setColors( const std::vector<Util::Color::Rgba> & p_colors );

		Util::Callback<> onChange;

	  private:
		VTX::Core::Struct::ColorLayout _layout;
	};

} // namespace VTX::App::Component::Representation

#endif
