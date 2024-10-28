#ifndef __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__
#define __VTX_APP_COMPONENT_REPRESENTATION_RENDER_SETTINGS__

#include "app/core/ecs/base_component.hpp"
#include <core/struct/render_settings.hpp>
#include <renderer/proxy/render_settings.hpp>
#include <renderer/settings.hpp>
#include <unordered_map>
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
			if constexpr ( S == Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY )
			{
				_settings.ssaoIntensity = p_value;
				_proxy->onSSAOIntensity( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::BLUR_SIZE )
			{
				_settings.blurSize = p_value;
				_proxy->onBlurSize( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND )
			{
				_settings.colorBackground = p_value;
				_proxy->onColorBackground( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_LIGHT )
			{
				_settings.colorLight = p_value;
				_proxy->onColorLight( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_FOG )
			{
				_settings.colorFog = p_value;
				_proxy->onColorFog( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHADING_MODE )
			{
				_settings.shadingMode = p_value;
				_proxy->onShadingMode( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR )
			{
				_settings.specularFactor = p_value;
				_proxy->onSpecularFactor( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHININESS )
			{
				_settings.shininess = p_value;
				_proxy->onShininess( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::TOON_STEPS )
			{
				_settings.toonSteps = p_value;
				_proxy->onToonSteps( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_NEAR )
			{
				_settings.fogNear = p_value;
				_proxy->onFogNear( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_FAR )
			{
				_settings.fogFar = p_value;
				_proxy->onFogFar( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_DENSITY )
			{
				_settings.fogDensity = p_value;
				_proxy->onFogDensity( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE )
			{
				_settings.colorOutline = p_value;
				_proxy->onColorOutline( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
			{
				_settings.outlineSensitivity = p_value;
				_proxy->onOutlineSensitivity( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS )
			{
				_settings.outlineThickness = p_value;
				_proxy->onOutlineThickness( p_value );
			}
			else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_SELECTION )
			{
				_settings.colorSelection = p_value;
				_proxy->onColorSelection( p_value );
			}
			// TODO: test this.
			else
			{
				static_assert( true, "Unknown render setting." );
			}

			getCallback<S, T>()( p_value );
		}

		template<Renderer::E_RENDER_SETTINGS S, typename T>
		Util::Callback<T> & getCallback()
		{
			static Util::Callback<T> callback;
			return callback;
		}

	  private:
		VTX::Core::Struct::RenderSettings _settings;
	};

} // namespace VTX::App::Component::Representation

#endif
