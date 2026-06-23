#ifndef __VTX_RENDERER_MATERIAL__
#define __VTX_RENDERER_MATERIAL__

#include <optional>
#include <string>
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
	 * @brief PBR metallic/roughness material descriptor.
	 */
	struct Material
	{
		std::string name;
		Vec3f		emissiveColor { 0.f };
		float		emissiveIntensity = MATERIAL_EMISSIVE_INTENSITY_DEFAULT;
		float		metallic		  = MATERIAL_METALLIC_DEFAULT;
		float		roughness		  = MATERIAL_ROUGHNESS_DEFAULT;
		float		opacity			  = MATERIAL_OPACITY_DEFAULT;

		std::optional<MaterialTexture> normalTexture;
		std::optional<MaterialTexture> metallicTexture;
		std::optional<MaterialTexture> roughnessTexture;
		std::optional<MaterialTexture> ambientOcclusionTexture;
		std::optional<MaterialTexture> emissiveTexture;

		bool doubleSided = false;
	};

	namespace Materials
	{
		inline const Material DEFAULT { .name = "Default" };
		inline const Material MATTE { .name = "Matte", .metallic = 0.f, .roughness = 0.9f };
		inline const Material PLASTIC { .name = "Plastic", .metallic = 0.f, .roughness = 0.35f };
		inline const Material POLISHED_METAL { .name = "Polished metal", .metallic = 1.f, .roughness = 0.1f };
		inline const Material BRUSHED_METAL { .name = "Brushed metal", .metallic = 1.f, .roughness = 0.45f };
	} // namespace Materials
} // namespace VTX::Renderer

#endif
