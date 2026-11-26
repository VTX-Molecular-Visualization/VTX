#ifndef VTX_APP_ACTION_GRAPHICS_CONFIG_HPP
#define VTX_APP_ACTION_GRAPHICS_CONFIG_HPP

#include "app/ecs.hpp"
#include "app/helper/preset.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <filesystem>
#include <optional>
#include <renderer/baker.hpp>
#include <renderer/graphics_config.hpp>
#include <util/string.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::GraphicsConfig
{
	namespace Detail
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

		inline std::optional<FilePath> prepareEnvironmentPath( const FilePath & p_path, const uint p_faceSize )
		{
			if ( p_path.empty() || not std::filesystem::exists( p_path ) )
			{
				return std::nullopt;
			}

			if ( Util::String::toLower( p_path.extension().string() ) == ".ktx2" )
			{
				return p_path;
			}
			if ( not Renderer::Baker::isEnvironmentMapFile( p_path ) )
			{
				return std::nullopt;
			}

			// FilePath ktxPath = SESSION().getHdrisDir() / FilePath( p_path.filename() ).replace_extension( ".ktx2" );
			// if ( ( not std::filesystem::exists( ktxPath )
			//	   || std::filesystem::last_write_time( p_path ) > std::filesystem::last_write_time( ktxPath ) )
			//	 && not Renderer::Baker::bakeEnvironmentMapToKtx( p_path, ktxPath, p_faceSize ) )
			// {
			//	return std::nullopt;
			// }

			// return ktxPath;

			return p_path;
		}
	} // namespace Detail

	/**
	 * @brief Change a render settings value.
	 */
	template<VTX::Renderer::E_GRAPHICS_CONFIG_VALUES S, typename T>
	struct Change
	{
		void execute( const Entity p_e, const T & p_value )
		{
			Helper::Preset::checkEditable( p_e );
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
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::LIGHT_INTENSITY )
					{
						p_config.shading.lightIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::AMBIENT_INTENSITY )
					{
						p_config.shading.ambientIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::BACKGROUND_MODE )
					{
						p_config.shading.backgroundMode = static_cast<Renderer::E_BACKGROUND_MODE>( p_value );
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
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_PATH )
					{
						p_config.shading.environmentPath
							= Detail::prepareEnvironmentPath( p_value, p_config.shading.environmentFaceSize );
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SKYBOX_INTENSITY )
					{
						p_config.shading.skyboxIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::IBL_INTENSITY )
					{
						p_config.shading.iblIntensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ENVIRONMENT_ROTATION )
					{
						p_config.shading.environmentRotation = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::TONE_MAPPING_MODE )
					{
						p_config.toneMapping.mode = static_cast<Renderer::E_TONE_MAPPING>( p_value );
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::TONE_MAPPING_EXPOSURE )
					{
						p_config.toneMapping.exposure = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SSAO )
					{
						p_config.ssao
							= p_value ? std::optional<Renderer::SSAOConfig> { Renderer::SSAOConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SSAO_INTENSITY )
					{
						Detail::ensure( p_config.ssao, Renderer::SSAOConfig {} ).intensity = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SSAO_METHOD )
					{
						Detail::ensure( p_config.ssao, Renderer::SSAOConfig {} ).method
							= static_cast<Renderer::E_SSAO_METHOD>( p_value );
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::SSAO_SCALE )
					{
						Detail::ensure( p_config.ssao, Renderer::SSAOConfig {} ).scale = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::BLUR_SIZE )
					{
						Detail::ensure( p_config.ssao, Renderer::SSAOConfig {} ).blurSize = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_FOG )
					{
						p_config.fog
							= p_value ? std::optional<Renderer::FogConfig> { Renderer::FogConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_FOG )
					{
						Detail::ensure( p_config.fog, Renderer::FogConfig {} ).color = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_NEAR )
					{
						Detail::ensure( p_config.fog, Renderer::FogConfig {} ).near = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_FAR )
					{
						Detail::ensure( p_config.fog, Renderer::FogConfig {} ).far = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::FOG_DENSITY )
					{
						Detail::ensure( p_config.fog, Renderer::FogConfig {} ).density = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_OUTLINE )
					{
						p_config.outline
							= p_value ? std::optional<
											Renderer::OutlineConfig> { Renderer::OutlineConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_OUTLINE )
					{
						Detail::ensure( p_config.outline, Renderer::OutlineConfig {} ).color = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_SENSITIVITY )
					{
						Detail::ensure( p_config.outline, Renderer::OutlineConfig {} ).sensitivity
							= p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::OUTLINE_THICKNESS )
					{
						Detail::ensure( p_config.outline, Renderer::OutlineConfig {} ).thickness
							= p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CHROMAB )
					{
						p_config.chromaticAberration
							= p_value ? std::optional<Renderer::ChromaticAberrationConfig> { Renderer::ChromaticAberrationConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_RED )
					{
						Detail::ensure( p_config.chromaticAberration,
										Renderer::ChromaticAberrationConfig {} )
							.red = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_GREEN )
					{
						Detail::ensure( p_config.chromaticAberration,
										Renderer::ChromaticAberrationConfig {} )
							.green = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CHROMAB_BLUE )
					{
						Detail::ensure( p_config.chromaticAberration,
										Renderer::ChromaticAberrationConfig {} )
							.blue = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_PIXELIZE )
					{
						p_config.pixelize
							= p_value ? std::optional<
											Renderer::PixelizeConfig> { Renderer::PixelizeConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_SIZE )
					{
						Detail::ensure( p_config.pixelize, Renderer::PixelizeConfig {} ).size = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::PIXELIZE_BACKGROUND )
					{
						Detail::ensure( p_config.pixelize, Renderer::PixelizeConfig {} ).background
							= p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_CRT )
					{
						p_config.crt
							= p_value ? std::optional<Renderer::CRTConfig> { Renderer::CRTConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_X )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).curvatureX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_CURVATURE_Y )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).curvatureY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_RATIO )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).ratio = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_X )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).graninessX = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_GRANINESS_Y )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).graninessY = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_ROUNDNESS )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).vignetteRoundness
							= p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_VIGNETTE_INTENSITY )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).vignetteIntensity
							= p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::CRT_BRIGHTNESS )
					{
						Detail::ensure( p_config.crt, Renderer::CRTConfig {} ).brightness = p_value;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::ACTIVE_SELECTION )
					{
						p_config.selection
							= p_value ? std::optional<
											Renderer::SelectionConfig> { Renderer::SelectionConfig {} }
									  : std::nullopt;
					}
					else if constexpr ( S == Renderer::E_GRAPHICS_CONFIG_VALUES::COLOR_SELECTION )
					{
						Detail::ensure( p_config.selection, Renderer::SelectionConfig {} ).color
							= p_value;
					}
					else
					{
					}
				} );
		}
	};

} // namespace VTX::App::Action::GraphicsConfig
#endif
