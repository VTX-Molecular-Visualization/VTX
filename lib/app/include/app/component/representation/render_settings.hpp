#ifndef __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__
#define __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__

#include "app/core/ecs/base_component.hpp"
#include <core/struct/render_settings.hpp>
#include <renderer/proxy/render_settings.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class RenderSettings : public Core::ECS::BaseComponentProxy<Renderer::Proxy::RenderSettings>
	{
	  public:
		RenderSettings();

		void setupProxy() override;

		const VTX::Core::Struct::RenderSettings & getSettings() const { return _settings; }

		/*
		void setColor( const size_t p_index, const Util::Color::Rgba & p_color );
		void setColors( const std::vector<Util::Color::Rgba> & p_colors );
		*/

		template<Renderer::Proxy::E_RENDER_SETTINGS S, typename T>
		void set( const T & p_value )
		{
			if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::SSAO_INTENSITY )
			{
				_settings.ssaoIntensity = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::BLUR_SIZE )
			{
				_settings.blurSize = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::COLOR_BACKGROUND )
			{
				_settings.colorBackground = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::COLOR_LIGHT )
			{
				_settings.colorLight = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::COLOR_FOG )
			{
				_settings.colorFog = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::SHADING_MODE )
			{
				_settings.shadingMode = p_value;
				_proxy->onShadingMode( p_value );
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::SPECULAR_FACTOR )
			{
				_settings.specularFactor = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::SHININESS )
			{
				_settings.shininess = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::TOON_STEPS )
			{
				_settings.toonSteps = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::FOG_NEAR )
			{
				_settings.fogNear = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::FOG_FAR )
			{
				_settings.fogFar = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::FOG_DENSITY )
			{
				_settings.fogDensity = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::COLOR_OUTLINE )
			{
				_settings.colorOutline = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
			{
				_settings.outlineSensitivity = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::OUTLINE_THICKNESS )
			{
				_settings.outlineThickness = p_value;
			}
			else if constexpr ( S == Renderer::Proxy::E_RENDER_SETTINGS::COLOR_SELECTION )
			{
				_settings.colorSelection = p_value;
			}
		}

		Util::Callback<Renderer::Proxy::E_RENDER_SETTINGS> onChange;

	  private:
		VTX::Core::Struct::RenderSettings _settings;
	};

} // namespace VTX::App::Component::Representation

#endif
