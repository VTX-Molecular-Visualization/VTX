#ifndef __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__
#define __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__

#include "app/core/ecs/base_component.hpp"
#include <core/struct/render_settings.hpp>
#include <renderer/proxy/render_settings.hpp>
#include <renderer/settings.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class RenderSettings : public Core::ECS::BaseComponentProxy<Renderer::Proxy::RenderSettings>
	{
	  public:
		RenderSettings();

		void setupProxy() override;

		const VTX::Core::Struct::RenderSettings & getSettings() const { return _settings; }

		template<Renderer::E_RENDER_SETTINGS S, typename T>
		void set( const T p_value )
		{
			// SSAO.
			if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_SSAO )
			{
				_settings.activeSSAO = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY )
			{
				_settings.ssaoIntensity = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::BLUR_SIZE )
			{
				_settings.blurSize = p_value;
			}
			// Shading.
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHADING_MODE )
			{
				_settings.shadingMode = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_LIGHT )
			{
				_settings.colorLight = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND )
			{
				_settings.colorBackground = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR )
			{
				_settings.specularFactor = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHININESS )
			{
				_settings.shininess = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::TOON_STEPS )
			{
				_settings.toonSteps = p_value;
			}
			// Fog.
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_FOG )
			{
				_settings.activeFog = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_FOG )
			{
				_settings.colorFog = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_NEAR )
			{
				_settings.fogNear = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_FAR )
			{
				_settings.fogFar = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_DENSITY )
			{
				_settings.fogDensity = p_value;
			}
			// Outline.
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_OUTLINE )
			{
				_settings.activeOutline = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE )
			{
				_settings.colorOutline = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
			{
				_settings.outlineSensitivity = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS )
			{
				_settings.outlineThickness = p_value;
			}
			// Selection.
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_SELECTION )
			{
				_settings.activeSelection = p_value;
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_SELECTION )
			{
				_settings.colorSelection = p_value;
			}
			// TODO: test this.
			else
			{
				static_assert( true, "Unknown render setting." );
			}

			// Trigger UI.
			callback<S, T>()( p_value );

			// Trigger renderer.
			_proxy->onChange<S>()();
		}

		template<Renderer::E_RENDER_SETTINGS S, typename T>
		Util::Callback<const T> & callback()
		{
			static Util::Callback<const T> callback;
			return callback;
		}

	  private:
		VTX::Core::Struct::RenderSettings _settings;
	};

} // namespace VTX::App::Component::Representation

#endif
