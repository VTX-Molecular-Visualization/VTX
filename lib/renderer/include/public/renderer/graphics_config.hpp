#ifndef __VTX_RENDERER_GRAPHICS_CONFIG__
#define __VTX_RENDERER_GRAPHICS_CONFIG__

#include "renderer/builder/post_process/chromatic_aberration.hpp"
#include "renderer/builder/post_process/colorize.hpp"
#include "renderer/builder/post_process/crt.hpp"
#include "renderer/builder/post_process/outline.hpp"
#include "renderer/builder/post_process/pixelize.hpp"
#include "renderer/builder/post_process/selection.hpp"
#include "renderer/builder/post_process/shading.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include <optional>

namespace VTX::Renderer
{
	enum struct E_GRAPHICS_CONFIG_VALUES
	{
		ACTIVE_SSAO,
		SSAO_METHOD,
		SSAO_SCALE,
		SSAO_INTENSITY,
		BLUR_SIZE,

		SHADING_MODE,
		COLOR_LIGHT,
		COLOR_BACKGROUND,
		SPECULAR_FACTOR,
		SHININESS,
		TOON_STEPS,

		ACTIVE_FOG,
		COLOR_FOG,
		FOG_NEAR,
		FOG_FAR,
		FOG_DENSITY,

		ACTIVE_COLORIZE,
		COLOR_COLORIZE,

		ACTIVE_OUTLINE,
		COLOR_OUTLINE,
		OUTLINE_SENSITIVITY,
		OUTLINE_THICKNESS,

		ACTIVE_CHROMAB,
		CHROMAB_RED,
		CHROMAB_GREEN,
		CHROMAB_BLUE,

		ACTIVE_PIXELIZE,
		PIXELIZE_SIZE,
		PIXELIZE_BACKGROUND,

		ACTIVE_CRT,
		CRT_CURVATURE_X,
		CRT_CURVATURE_Y,
		CRT_RATIO,
		CRT_GRANINESS_X,
		CRT_GRANINESS_Y,
		CRT_VIGNETTE_ROUNDNESS,
		CRT_VIGNETTE_INTENSITY,
		CRT_BRIGHTNESS,

		ACTIVE_SELECTION,
		COLOR_SELECTION
	};

	struct GraphicsConfig
	{
		ShadingConfig							 shading;
		std::optional<SSAOConfig>				 ssao;
		std::optional<OutlineConfig>			 outline;
		std::optional<FogConfig>				 fog;
		std::optional<ColorizeConfig>			 colorize;
		std::optional<ChromaticAberrationConfig> chromaticAberration;
		std::optional<PixelizeConfig>			 pixelize;
		std::optional<CRTConfig>				 crt;
		std::optional<SelectionConfig>			 selection;
	};

	namespace GraphicsConfigs
	{
		inline const GraphicsConfig DEFAULT {
			SHADING_DEFAULT,
			ACTIVE_SSAO_DEFAULT ? std::optional<SSAOConfig> { SSAO_DEFAULT } : std::nullopt,
			ACTIVE_OUTLINE_DEFAULT ? std::optional<OutlineConfig> { OUTLINE_DEFAULT } : std::nullopt,
			ACTIVE_FOG_DEFAULT ? std::optional<FogConfig> { FOG_DEFAULT } : std::nullopt,
			ACTIVE_COLORIZE_DEFAULT ? std::optional<ColorizeConfig> { COLORIZE_DEFAULT } : std::nullopt,
			ACTIVE_CHROMAB_DEFAULT ? std::optional<ChromaticAberrationConfig> { CHROMATIC_ABERRATION_DEFAULT }
								   : std::nullopt,
			ACTIVE_PIXELIZE_DEFAULT ? std::optional<PixelizeConfig> { PIXELIZE_DEFAULT } : std::nullopt,
			ACTIVE_CRT_DEFAULT ? std::optional<CRTConfig> { CRT_DEFAULT } : std::nullopt,
			ACTIVE_SELECTION_DEFAULT ? std::optional<SelectionConfig> { SELECTION_DEFAULT } : std::nullopt
		};

		inline const GraphicsConfig PRESENTATION {
			ShadingConfig { E_SHADING::GLOSSY,
							Util::Color::Rgba( 255, 255, 255 ),
							Util::Color::Rgba( 18, 20, 24 ),
							0.55f,
							56.f,
							TOON_STEPS_DEFAULT },
			SSAOConfig { SSAO_METHOD_DEFAULT, 4.f, SSAO_RADIUS_DEFAULT, SSAO_SCALE_DEFAULT, 21.f },
			std::nullopt,
			std::nullopt,
			ACTIVE_COLORIZE_DEFAULT ? std::optional<ColorizeConfig> { COLORIZE_DEFAULT } : std::nullopt,
			ACTIVE_CHROMAB_DEFAULT ? std::optional<ChromaticAberrationConfig> { CHROMATIC_ABERRATION_DEFAULT }
								   : std::nullopt,
			ACTIVE_PIXELIZE_DEFAULT ? std::optional<PixelizeConfig> { PIXELIZE_DEFAULT } : std::nullopt,
			ACTIVE_CRT_DEFAULT ? std::optional<CRTConfig> { CRT_DEFAULT } : std::nullopt,
			SelectionConfig { Util::Color::Rgba( 80, 255, 120 ) }
		};

		inline const GraphicsConfig ILLUSTRATION {
			ShadingConfig { E_SHADING::TOON,
							Util::Color::Rgba( 255, 255, 255 ),
							Util::Color::Rgba( 245, 245, 242 ),
							0.f,
							0.f,
							4 },
			SSAOConfig { SSAO_METHOD_DEFAULT, 2.5f, SSAO_RADIUS_DEFAULT, SSAO_SCALE_DEFAULT, 11.f },
			OutlineConfig { Util::Color::Rgba( 28, 28, 28 ), 0.35f, 2 },
			std::nullopt,
			ACTIVE_COLORIZE_DEFAULT ? std::optional<ColorizeConfig> { COLORIZE_DEFAULT } : std::nullopt,
			ACTIVE_CHROMAB_DEFAULT ? std::optional<ChromaticAberrationConfig> { CHROMATIC_ABERRATION_DEFAULT }
								   : std::nullopt,
			ACTIVE_PIXELIZE_DEFAULT ? std::optional<PixelizeConfig> { PIXELIZE_DEFAULT } : std::nullopt,
			ACTIVE_CRT_DEFAULT ? std::optional<CRTConfig> { CRT_DEFAULT } : std::nullopt,
			SelectionConfig { Util::Color::Rgba( 255, 64, 64 ) }
		};

	}; // namespace GraphicsConfigs
} // namespace VTX::Renderer

#endif
