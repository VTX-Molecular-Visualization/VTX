#ifndef __VTX_RENDERER_MATERIAL__
#define __VTX_RENDERER_MATERIAL__

#include <optional>
#include <string>
#include <util/color/rgba.hpp>
#include <util/types.hpp>

namespace VTX::Renderer
{
	constexpr float MATERIAL_METALLIC_DEFAULT			= 0.f;
	constexpr float MATERIAL_ROUGHNESS_DEFAULT			= 1.f;
	constexpr float MATERIAL_OPACITY_DEFAULT			= 1.f;
	constexpr float MATERIAL_EMISSIVE_INTENSITY_DEFAULT = 1.f;

	struct MaterialTexture
	{
		std::string path;
		uint32_t	uvIndex = 0;
	};

	/**
	 * @brief Material descriptor matching Assimp's common PBR metallic/roughness properties.
	 */
	struct Material
	{
		std::string		  name;
		Util::Color::Rgba baseColor { 1.f, 1.f, 1.f, 1.f };
		Vec3f			  emissiveColor { 0.f };
		float			  emissiveIntensity = MATERIAL_EMISSIVE_INTENSITY_DEFAULT;
		float			  metallic			= MATERIAL_METALLIC_DEFAULT;
		float			  roughness			= MATERIAL_ROUGHNESS_DEFAULT;
		float			  opacity			= MATERIAL_OPACITY_DEFAULT;

		std::optional<MaterialTexture> baseColorTexture;
		std::optional<MaterialTexture> normalTexture;
		std::optional<MaterialTexture> metallicTexture;
		std::optional<MaterialTexture> roughnessTexture;
		std::optional<MaterialTexture> ambientOcclusionTexture;
		std::optional<MaterialTexture> emissiveTexture;

		bool doubleSided = false;
	};
} // namespace VTX::Renderer

#endif
