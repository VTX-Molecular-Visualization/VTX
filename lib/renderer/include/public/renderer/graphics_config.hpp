#ifndef VTX_RENDERER_GRAPHICS_CONFIG_HPP
#define VTX_RENDERER_GRAPHICS_CONFIG_HPP

#include "renderer/builder/post_process/chromatic_aberration.hpp"
#include "renderer/builder/post_process/crt.hpp"
#include "renderer/builder/post_process/outline.hpp"
#include "renderer/builder/post_process/pixelize.hpp"
#include "renderer/builder/post_process/selection.hpp"
#include "renderer/builder/post_process/shading.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include "renderer/builder/post_process/tone_mapping.hpp"
#include <optional>

namespace VTX::Renderer
{
	enum struct E_GRAPHICS_CONFIG_VALUES : uint8_t
	{
		ACTIVE_SSAO,
		SSAO_METHOD,
		SSAO_SCALE,
		SSAO_INTENSITY,
		BLUR_SIZE,

		SHADING_MODE,
		COLOR_LIGHT,
		LIGHT_INTENSITY,
		AMBIENT_INTENSITY,
		BACKGROUND_MODE,
		COLOR_BACKGROUND,
		SPECULAR_FACTOR,
		SHININESS,
		TOON_STEPS,
		MATERIAL,
		MATERIAL_METALLIC,
		MATERIAL_ROUGHNESS,
		MATERIAL_EMISSIVE_INTENSITY,
		MATERIAL_TEXTURE_SCALE,
		MATERIAL_ALBEDO_TEXTURE,
		MATERIAL_NORMAL_TEXTURE,
		MATERIAL_METALLIC_TEXTURE,
		MATERIAL_ROUGHNESS_TEXTURE,
		MATERIAL_AMBIENT_OCCLUSION_TEXTURE,
		MATERIAL_EMISSIVE_TEXTURE,
		ENVIRONMENT_PATH,
		SKYBOX_INTENSITY,
		IBL_INTENSITY,
		ENVIRONMENT_ROTATION,
		TONE_MAPPING_MODE,
		TONE_MAPPING_EXPOSURE,

		ACTIVE_FOG,
		COLOR_FOG,
		FOG_NEAR,
		FOG_FAR,
		FOG_DENSITY,

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
		ShadingConfig			  shading;
		std::optional<SSAOConfig> ssao = ACTIVE_SSAO_DEFAULT ? std::optional<SSAOConfig> { SSAOConfig {} } : std::nullopt;
		std::optional<OutlineConfig> outline
			= ACTIVE_OUTLINE_DEFAULT ? std::optional<OutlineConfig> { OutlineConfig {} } : std::nullopt;
		std::optional<FogConfig> fog = ACTIVE_FOG_DEFAULT ? std::optional<FogConfig> { FogConfig {} } : std::nullopt;
		std::optional<ChromaticAberrationConfig> chromaticAberration
			= ACTIVE_CHROMAB_DEFAULT ? std::optional<ChromaticAberrationConfig> { ChromaticAberrationConfig {} }
									 : std::nullopt;
		std::optional<PixelizeConfig> pixelize
			= ACTIVE_PIXELIZE_DEFAULT ? std::optional<PixelizeConfig> { PixelizeConfig {} } : std::nullopt;
		std::optional<CRTConfig> crt = ACTIVE_CRT_DEFAULT ? std::optional<CRTConfig> { CRTConfig {} } : std::nullopt;
		std::optional<SelectionConfig> selection
			= ACTIVE_SELECTION_DEFAULT ? std::optional<SelectionConfig> { SelectionConfig {} } : std::nullopt;
		ToneMappingConfig toneMapping = GraphicsConfigs::TONE_MAPPING_DEFAULT;
	};
} // namespace VTX::Renderer

#endif
