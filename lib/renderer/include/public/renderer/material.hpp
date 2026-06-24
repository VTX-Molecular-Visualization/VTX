#ifndef __VTX_RENDERER_MATERIAL__
#define __VTX_RENDERER_MATERIAL__

#include <cstddef>
#include <optional>
#include <string>
#include <util/types.hpp>

namespace VTX::Renderer
{
	constexpr float MATERIAL_METALLIC_DEFAULT			= 0.f;
	constexpr float MATERIAL_METALLIC_MIN				= 0.f;
	constexpr float MATERIAL_METALLIC_MAX				= 1.f;
	constexpr float MATERIAL_ROUGHNESS_DEFAULT			= 1.f;
	constexpr float MATERIAL_ROUGHNESS_MIN				= 0.f;
	constexpr float MATERIAL_ROUGHNESS_MAX				= 1.f;
	constexpr float MATERIAL_OPACITY_DEFAULT			= 1.f;
	constexpr float MATERIAL_OPACITY_MIN				= 0.f;
	constexpr float MATERIAL_OPACITY_MAX				= 1.f;
	constexpr float MATERIAL_EMISSIVE_INTENSITY_DEFAULT = 0.f;
	constexpr float MATERIAL_EMISSIVE_INTENSITY_MIN		= 0.f;
	constexpr float MATERIAL_EMISSIVE_INTENSITY_MAX		= 10.f;
	constexpr float MATERIAL_TEXTURE_SCALE_DEFAULT		= 1.f;
	constexpr float MATERIAL_TEXTURE_SCALE_MIN			= 0.01f;
	constexpr float MATERIAL_TEXTURE_SCALE_MAX			= 10.f;

	struct MaterialTexture
	{
		std::string path;
		uint32_t	uvIndex = 0;

		bool operator==( const MaterialTexture & ) const = default;
	};

	/**
	 * @brief PBR metallic/roughness material descriptor.
	 */
	struct Material
	{
		enum struct E_TEXTURE : uint8_t
		{
			ALBEDO,
			NORMAL,
			METALLIC,
			ROUGHNESS,
			AMBIENT_OCCLUSION,
			EMISSIVE,
			COUNT
		};

		std::string name;
		Vec3f		emissiveColor { 0.f };
		float		emissiveIntensity = MATERIAL_EMISSIVE_INTENSITY_DEFAULT;
		float		metallic		  = MATERIAL_METALLIC_DEFAULT;
		float		roughness		  = MATERIAL_ROUGHNESS_DEFAULT;
		float		opacity			  = MATERIAL_OPACITY_DEFAULT;
		float		textureScale	  = MATERIAL_TEXTURE_SCALE_DEFAULT;

		std::optional<MaterialTexture> albedoTexture;
		std::optional<MaterialTexture> normalTexture;
		std::optional<MaterialTexture> metallicTexture;
		std::optional<MaterialTexture> roughnessTexture;
		std::optional<MaterialTexture> ambientOcclusionTexture;
		std::optional<MaterialTexture> emissiveTexture;

		bool doubleSided = false;

		uint32_t textureMask() const noexcept
		{
			return ( uint32_t( albedoTexture.has_value() ) << uint8_t( E_TEXTURE::ALBEDO ) )
				   | ( uint32_t( normalTexture.has_value() ) << uint8_t( E_TEXTURE::NORMAL ) )
				   | ( uint32_t( metallicTexture.has_value() ) << uint8_t( E_TEXTURE::METALLIC ) )
				   | ( uint32_t( roughnessTexture.has_value() ) << uint8_t( E_TEXTURE::ROUGHNESS ) )
				   | ( uint32_t( ambientOcclusionTexture.has_value() ) << uint8_t( E_TEXTURE::AMBIENT_OCCLUSION ) )
				   | ( uint32_t( emissiveTexture.has_value() ) << uint8_t( E_TEXTURE::EMISSIVE ) );
		}
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
