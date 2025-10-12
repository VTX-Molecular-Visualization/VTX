#ifndef __VTX_APP_LIBRARY_PRESET_RENDER_SETTINGS__
#define __VTX_APP_LIBRARY_PRESET_RENDER_SETTINGS__

#include "app/library/base_preset.hpp"
#include <renderer/settings.hpp>
#include <util/constants.hpp>
#include <util/math.hpp>

namespace VTX::App::Library::Preset
{

	class RenderSettings : public BasePreset<Renderer::RenderSettings>
	{
	  public:
		RenderSettings();

		void save() override;
		void load() override;

		template<Renderer::E_RENDER_SETTINGS S, typename T>
		void setValue( const T p_value )
		{
			using namespace VTX::Renderer;
			using namespace VTX::Renderer::Settings;
			using namespace Util::Math;

			T value = p_value;

			// SSAO.
			if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_SSAO )
			{
				_data.activeSSAO = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SSAO_INTENSITY )
			{
				value				= clamp( value, SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX );
				_data.ssaoIntensity = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::BLUR_SIZE )
			{
				value		   = clamp( value, BLUR_SIZE_MIN, BLUR_SIZE_MAX );
				_data.blurSize = value;
			}
			// Shading.
			else if constexpr ( S == E_RENDER_SETTINGS::SHADING_MODE )
			{
				_data.shadingMode = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_LIGHT )
			{
				_data.colorLight = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_BACKGROUND )
			{
				_data.colorBackground = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SPECULAR_FACTOR )
			{
				value				 = clamp( value, SPECULAR_FACTOR_MIN, SPECULAR_FACTOR_MAX );
				_data.specularFactor = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::SHININESS )
			{
				value			= clamp( value, SHININESS_MIN, SHININESS_MAX );
				_data.shininess = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::TOON_STEPS )
			{
				value			= clamp( value, TOON_STEPS_MIN, TOON_STEPS_MAX );
				_data.toonSteps = value;
			}
			// Fog.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_FOG )
			{
				_data.activeFog = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_FOG )
			{
				_data.colorFog = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_NEAR )
			{
				value		  = clamp( value, FOG_NEAR_MIN, FOG_NEAR_MAX );
				_data.fogNear = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_FAR )
			{
				value		 = clamp( value, FOG_FAR_MIN, FOG_FAR_MAX );
				_data.fogFar = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::FOG_DENSITY )
			{
				value			 = clamp( value, FOG_DENSITY_MIN, FOG_DENSITY_MAX );
				_data.fogDensity = value;
			}
			// Outline.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_OUTLINE )
			{
				_data.activeOutline = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_OUTLINE )
			{
				_data.colorOutline = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
			{
				value					 = clamp( value, OUTLINE_SENSITIVITY_MIN, OUTLINE_SENSITIVITY_MAX );
				_data.outlineSensitivity = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::OUTLINE_THICKNESS )
			{
				value				   = clamp( value, OUTLINE_THICKNESS_MIN, OUTLINE_THICKNESS_MAX );
				_data.outlineThickness = value;
			}
			// Selection.
			else if constexpr ( S == E_RENDER_SETTINGS::ACTIVE_SELECTION )
			{
				_data.activeSelection = value;
			}
			else if constexpr ( S == E_RENDER_SETTINGS::COLOR_SELECTION )
			{
				_data.colorSelection = value;
			}
			else
			{
				static_assert( std::is_same_v<T, void>, "Unknown render setting." );
			}

			getCallback<S, T>()( value );
		}
	};
} // namespace VTX::App::Library::Preset

#endif
