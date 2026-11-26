#ifndef VTX_APP_SERIALIZATION_RENDERER_HPP
#define VTX_APP_SERIALIZATION_RENDERER_HPP

#include <renderer/color.hpp>
#include <renderer/graphics_config.hpp>
#include <renderer/material.hpp>
#include <renderer/representation.hpp>
#include <util/json.hpp>

/**
 * @brief Generate to_json and from_json functions for renderer types.
 */
namespace VTX::Renderer
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		Representation,
		hasSphere,
		radiusSphereFixed,
		radiusSphereAdd,
		isRadiusSphereFixed,
		hasCylinder,
		radiusCylinder,
		cylinderColorBlending,
		hasRibbon,
		ribbonColorBlending,
		hasSes,
		sesComputeMode,
		sesProbeRadius
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( MaterialTexture, path, uvIndex )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		Material,
		emissiveColor,
		emissiveIntensity,
		metallic,
		roughness,
		opacity,
		textureScale,
		albedoTexture,
		normalTexture,
		metallicTexture,
		roughnessTexture,
		ambientOcclusionTexture,
		emissiveTexture,
		doubleSided
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		ShadingConfig,
		mode,
		colorLight,
		colorBackground,
		specularFactor,
		shininess,
		toonSteps,
		lightIntensity,
		ambientIntensity,
		backgroundMode,
		environmentPath,
		environmentFaceSize,
		skyboxIntensity,
		iblIntensity,
		environmentRotation
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( SSAOConfig, method, intensity, radius, scale, blurSize )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( OutlineConfig, color, sensitivity, thickness )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( FogConfig, color, near, far, density )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( ChromaticAberrationConfig, red, green, blue )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( PixelizeConfig, size, background )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		CRTConfig,
		curvatureX,
		curvatureY,
		ratio,
		graninessX,
		graninessY,
		vignetteRoundness,
		vignetteIntensity,
		brightness
	)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( SelectionConfig, color )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT( ToneMappingConfig, mode, exposure )

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
		GraphicsConfig,
		shading,
		ssao,
		outline,
		fog,
		chromaticAberration,
		pixelize,
		crt,
		selection,
		toneMapping
	)
} // namespace VTX::Renderer

namespace VTX::Renderer::Color
{
	/**
	 * @brief Serialize to_json and from_json functions for color layout.
	 */
	void to_json( Util::Json::Document & p_json, const Layout & p_layout );
	void from_json( const Util::Json::Document & p_json, Layout & p_layout );
} // namespace VTX::Renderer::Color

#endif
