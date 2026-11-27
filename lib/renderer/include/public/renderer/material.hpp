#ifndef VTX_RENDERER_MATERIAL_HPP
#define VTX_RENDERER_MATERIAL_HPP

#include <cstddef>
#include <optional>
#include <string>
#include <util/math.hpp>
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

	enum struct E_MATERIAL_VALUES : uint8_t
	{
		METALLIC,
		ROUGHNESS,
		EMISSIVE_INTENSITY,
		TEXTURE_SCALE,
		ALBEDO_TEXTURE,
		NORMAL_TEXTURE,
		METALLIC_TEXTURE,
		ROUGHNESS_TEXTURE,
		AMBIENT_OCCLUSION_TEXTURE,
		EMISSIVE_TEXTURE
	};

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

		void clamp()
		{
			emissiveIntensity
				= Util::Math::clamp( emissiveIntensity, MATERIAL_EMISSIVE_INTENSITY_MIN, MATERIAL_EMISSIVE_INTENSITY_MAX );
			metallic	 = Util::Math::clamp( metallic, MATERIAL_METALLIC_MIN, MATERIAL_METALLIC_MAX );
			roughness	 = Util::Math::clamp( roughness, MATERIAL_ROUGHNESS_MIN, MATERIAL_ROUGHNESS_MAX );
			opacity		 = Util::Math::clamp( opacity, MATERIAL_OPACITY_MIN, MATERIAL_OPACITY_MAX );
			textureScale = Util::Math::clamp( textureScale, MATERIAL_TEXTURE_SCALE_MIN, MATERIAL_TEXTURE_SCALE_MAX );
		}

		uint32_t textureMask() const noexcept
		{
			return ( static_cast<uint32_t>( albedoTexture.has_value() ) << static_cast<uint8_t>( E_TEXTURE::ALBEDO ) )
				   | ( static_cast<uint32_t>( normalTexture.has_value() ) << static_cast<uint8_t>( E_TEXTURE::NORMAL ) )
				   | ( static_cast<uint32_t>( metallicTexture.has_value() )
					   << static_cast<uint8_t>( E_TEXTURE::METALLIC ) )
				   | ( static_cast<uint32_t>( roughnessTexture.has_value() )
					   << static_cast<uint8_t>( E_TEXTURE::ROUGHNESS ) )
				   | ( static_cast<uint32_t>( ambientOcclusionTexture.has_value() )
					   << static_cast<uint8_t>( E_TEXTURE::AMBIENT_OCCLUSION ) )
				   | ( static_cast<uint32_t>( emissiveTexture.has_value() )
					   << static_cast<uint8_t>( E_TEXTURE::EMISSIVE ) );
		}
	};
} // namespace VTX::Renderer

#endif
