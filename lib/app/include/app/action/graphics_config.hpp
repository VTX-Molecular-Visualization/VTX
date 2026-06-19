#ifndef __VTX_APP_ACTION_GRAPHICS_CONFIG__
#define __VTX_APP_ACTION_GRAPHICS_CONFIG__

#include "app/ecs.hpp"
#include <optional>
#include <renderer/graphics_config.hpp>

namespace VTX::App::Action::GraphicsConfig
{
	namespace
	{
		template<typename T>
		T & ensure( std::optional<T> & p_config, const T & p_default )
		{
			if ( not p_config )
			{
				p_config = p_default;
			}

			return *p_config;
		}
	} // namespace

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
						p_config.shading.mode = static_cast<Renderer::E_SHADING>( p_value );
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_LIGHT )
					{
						p_config.shading.colorLight = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_BACKGROUND )
					{
						p_config.shading.colorBackground = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SPECULAR_FACTOR )
					{
						p_config.shading.specularFactor = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SHININESS )
					{
						p_config.shading.shininess = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::TOON_STEPS )
					{
						p_config.shading.toonSteps = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO )
					{
						p_config.ssao
							= p_value ? std::optional<Renderer::SSAOConfig> { Renderer::GraphicsConfigs::SSAO_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY )
					{
						ensure( p_config.ssao, Renderer::GraphicsConfigs::SSAO_DEFAULT ).intensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE )
					{
						ensure( p_config.ssao, Renderer::GraphicsConfigs::SSAO_DEFAULT ).blurSize = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_FOG )
					{
						p_config.fog
							= p_value ? std::optional<Renderer::FogConfig> { Renderer::GraphicsConfigs::FOG_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_FOG )
					{
						ensure( p_config.fog, Renderer::GraphicsConfigs::FOG_DEFAULT ).color = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_NEAR )
					{
						ensure( p_config.fog, Renderer::GraphicsConfigs::FOG_DEFAULT ).near = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_FAR )
					{
						ensure( p_config.fog, Renderer::GraphicsConfigs::FOG_DEFAULT ).far = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_DENSITY )
					{
						ensure( p_config.fog, Renderer::GraphicsConfigs::FOG_DEFAULT ).density = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_OUTLINE )
					{
						p_config.outline
							= p_value ? std::optional<
											Renderer::OutlineConfig> { Renderer::GraphicsConfigs::OUTLINE_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_OUTLINE )
					{
						ensure( p_config.outline, Renderer::GraphicsConfigs::OUTLINE_DEFAULT ).color = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_SENSITIVITY )
					{
						ensure( p_config.outline, Renderer::GraphicsConfigs::OUTLINE_DEFAULT ).sensitivity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_THICKNESS )
					{
						ensure( p_config.outline, Renderer::GraphicsConfigs::OUTLINE_DEFAULT ).thickness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CHROMAB )
					{
						p_config.chromaticAberration
							= p_value ? std::optional<
											Renderer::ChromaticAberrationConfig> { Renderer::GraphicsConfigs::
																					   CHROMATIC_ABERRATION_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_RED )
					{
						ensure( p_config.chromaticAberration, Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT )
							.red = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_GREEN )
					{
						ensure( p_config.chromaticAberration, Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT )
							.green = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_BLUE )
					{
						ensure( p_config.chromaticAberration, Renderer::GraphicsConfigs::CHROMATIC_ABERRATION_DEFAULT )
							.blue = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_PIXELIZE )
					{
						p_config.pixelize
							= p_value ? std::optional<
											Renderer::PixelizeConfig> { Renderer::GraphicsConfigs::PIXELIZE_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_SIZE )
					{
						ensure( p_config.pixelize, Renderer::GraphicsConfigs::PIXELIZE_DEFAULT ).size = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_BACKGROUND )
					{
						ensure( p_config.pixelize, Renderer::GraphicsConfigs::PIXELIZE_DEFAULT ).background = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CRT )
					{
						p_config.crt
							= p_value ? std::optional<Renderer::CRTConfig> { Renderer::GraphicsConfigs::CRT_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_X )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).curvatureX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_Y )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).curvatureY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_RATIO )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).ratio = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_X )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).graninessX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_Y )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).graninessY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_ROUNDNESS )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).vignetteRoundness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_INTENSITY )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).vignetteIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_BRIGHTNESS )
					{
						ensure( p_config.crt, Renderer::GraphicsConfigs::CRT_DEFAULT ).brightness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SELECTION )
					{
						p_config.selection
							= p_value ? std::optional<
											Renderer::SelectionConfig> { Renderer::GraphicsConfigs::SELECTION_DEFAULT }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_SELECTION )
					{
						ensure( p_config.selection, Renderer::GraphicsConfigs::SELECTION_DEFAULT ).color = p_value;
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
