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

		const VTX::Core::Struct::RenderSettings & getSettings() const { return _representation; }

		template<Renderer::Proxy::E_RENDER_SETTINGS S, typename T>
		void set( const T p_value )
		{
			using namespace Renderer::Proxy;

			// SSAO.
			if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_SSAO )
			{
				_representation.activeSSAO = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SSAO_INTENSITY )
			{
				_representation.ssaoIntensity = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::BLUR_SIZE )
			{
				_representation.blurSize = p_value;
			}
			// Shading.
			else if constexpr ( S == E_RENDER_SETTINGS::SHADING_MODE )
			{
				_representation.shadingMode = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_LIGHT )
			{
				_representation.colorLight = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_BACKGROUND )
			{
				_representation.colorBackground = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SPECULAR_FACTOR )
			{
				_representation.specularFactor = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SHININESS )
			{
				_representation.shininess = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::TOON_STEPS )
			{
				_representation.toonSteps = p_value;
			}
			// Fog.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_FOG )
			{
				_representation.activeFog = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_FOG )
			{
				_representation.colorFog = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_NEAR )
			{
				_representation.fogNear = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_FAR )
			{
				_representation.fogFar = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_DENSITY )
			{
				_representation.fogDensity = p_value;
			}
			// Outline.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_OUTLINE )
			{
				_representation.activeOutline = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_OUTLINE )
			{
				_representation.colorOutline = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
			{
				_representation.outlineSensitivity = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::OUTLINE_THICKNESS )
			{
				_representation.outlineThickness = p_value;
			}
			// Selection.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_SELECTION )
			{
				_representation.activeSelection = p_value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_SELECTION )
			{
				_representation.colorSelection = p_value;
			}
			else
			{
				static_assert( true, "Unknown render setting." );
			}

			// Trigger UI.
			callback<S, T>()( p_value );

			// Trigger renderer.
			_proxy->onChange<S, T>()( p_value );
		}

		template<Renderer::Proxy::E_RENDER_SETTINGS S, typename T>
		Util::Callback<const T> & callback()
		{
			static Util::Callback<const T> callback;
			return callback;
		}

	  private:
		VTX::Core::Struct::RenderSettings _representation;
	};

} // namespace VTX::App::Component::Representation

#endif
