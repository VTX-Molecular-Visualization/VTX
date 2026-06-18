#ifndef __VTX_APP_ACTION_GRAPHICS_CONFIG__
#define __VTX_APP_ACTION_GRAPHICS_CONFIG__

#include "app/ecs.hpp"
#include <renderer/graphics_config.hpp>

namespace VTX::App::Action::GraphicsConfig
{
	/**
	 * @brief Change a render settings value.
	 */
	template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
	struct Change
	{
		void execute( const Entity p_e, const T & p_value )
		{
			REG().patch<Renderer::GraphicsConfig>(
				p_e,
				[ p_value ]( Renderer::GraphicsConfig & p_config )
				{
					if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SHADING_MODE )
					{
						p_config.shadingMode = static_cast<Renderer::E_SHADING>( p_value );
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_LIGHT )
					{
						p_config.colorLight = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_BACKGROUND )
					{
						p_config.colorBackground = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SPECULAR_FACTOR )
					{
						p_config.specularFactor = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SHININESS )
					{
						p_config.shininess = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::TOON_STEPS )
					{
						p_config.toonSteps = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO )
					{
						p_config.activeSSAO = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY )
					{
						p_config.ssaoIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE )
					{
						p_config.blurSize = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_FOG )
					{
						p_config.activeFog = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_FOG )
					{
						p_config.colorFog = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_NEAR )
					{
						p_config.fogNear = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_FAR )
					{
						p_config.fogFar = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_DENSITY )
					{
						p_config.fogDensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_OUTLINE )
					{
						p_config.activeOutline = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_OUTLINE )
					{
						p_config.colorOutline = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_SENSITIVITY )
					{
						p_config.outlineSensitivity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_THICKNESS )
					{
						p_config.outlineThickness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CHROMAB )
					{
						p_config.activeChromaticAberration = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_RED )
					{
						p_config.chromaticAberrationRed = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_GREEN )
					{
						p_config.chromaticAberrationGreen = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_BLUE )
					{
						p_config.chromaticAberrationBlue = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_PIXELIZE )
					{
						p_config.activePixelize = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_SIZE )
					{
						p_config.pixelizeSize = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_BACKGROUND )
					{
						p_config.pixelizeBackground = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CRT )
					{
						p_config.activeCRT = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_X )
					{
						p_config.crtCurvatureX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_Y )
					{
						p_config.crtCurvatureY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_RATIO )
					{
						p_config.crtRatio = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_X )
					{
						p_config.crtGraninessX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_Y )
					{
						p_config.crtGraninessY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_ROUNDNESS )
					{
						p_config.crtVignetteRoundness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_INTENSITY )
					{
						p_config.crtVignetteIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_BRIGHTNESS )
					{
						p_config.crtBrightness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SELECTION )
					{
						p_config.activeSelection = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_SELECTION )
					{
						p_config.colorSelection = p_value;
					}
					else
					{
					}
				}
			);
		}
	};

} // namespace VTX::App::Action::GraphicsConfig
#endif
