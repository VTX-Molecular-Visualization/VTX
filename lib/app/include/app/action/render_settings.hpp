#ifndef __VTX_APP_ACTION_RENDER_SETTINGS__
#define __VTX_APP_ACTION_RENDER_SETTINGS__

#include "app/ecs.hpp"
#include <renderer/settings.hpp>

namespace VTX::App::Action::RenderSettings
{
	/**
	 * @brief Change a render settings value.
	 */
	template<VTX::Renderer::E_RENDER_SETTINGS S, typename T>
	struct Change
	{
		void execute( const ECS::Entity p_e, const T & p_value )
		{
			REG().patch<Renderer::RenderSettings>(
				p_e,
				[ p_value ]( Renderer::RenderSettings & p_settings )
				{
					if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHADING_MODE )
					{
						p_settings.shadingMode = static_cast<Renderer::E_SHADING>( p_value );
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_LIGHT )
					{
						p_settings.colorLight = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_BACKGROUND )
					{
						p_settings.colorBackground = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SPECULAR_FACTOR )
					{
						p_settings.specularFactor = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SHININESS )
					{
						p_settings.shininess = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::TOON_STEPS )
					{
						p_settings.toonSteps = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_SSAO )
					{
						p_settings.activeSSAO = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::SSAO_INTENSITY )
					{
						p_settings.ssaoIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::BLUR_SIZE )
					{
						p_settings.blurSize = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_FOG )
					{
						p_settings.activeFog = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_FOG )
					{
						p_settings.colorFog = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_NEAR )
					{
						p_settings.fogNear = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_FAR )
					{
						p_settings.fogFar = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::FOG_DENSITY )
					{
						p_settings.fogDensity = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_OUTLINE )
					{
						p_settings.activeOutline = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_OUTLINE )
					{
						p_settings.colorOutline = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_SENSITIVITY )
					{
						p_settings.outlineSensitivity = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::OUTLINE_THICKNESS )
					{
						p_settings.outlineThickness = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::ACTIVE_SELECTION )
					{
						p_settings.activeSelection = p_value;
					}
					else if constexpr ( S == Renderer::E_RENDER_SETTINGS::COLOR_SELECTION )
					{
						p_settings.colorSelection = p_value;
					}
					else
					{
					}
				}
			);
		}
	};

} // namespace VTX::App::Action::RenderSettings
#endif
