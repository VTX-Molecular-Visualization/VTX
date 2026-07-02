#include "renderer/builder/post_process/shading.hpp"
#include "renderer/baker.hpp"
#include <span>
#include <stdexcept>
#include <util/image.hpp>

namespace VTX::Renderer::Builder::PostProcess
{
	namespace
	{
		std::vector<Baker::EnvironmentTexel> _buildCubemap( const ShadingConfig & p_config )
		{
			if ( not p_config.environmentPath )
			{
				return {};
			}

			return Baker::loadEnvironmentCubemapKtx( *p_config.environmentPath, p_config.environmentFaceSize );
		}

		const std::optional<MaterialTexture> & _materialTexture(
			const Material &		  p_material,
			const Material::E_TEXTURE p_texture
		)
		{
			switch ( p_texture )
			{
			case Material::E_TEXTURE::ALBEDO: return p_material.albedoTexture;
			case Material::E_TEXTURE::NORMAL: return p_material.normalTexture;
			case Material::E_TEXTURE::METALLIC: return p_material.metallicTexture;
			case Material::E_TEXTURE::ROUGHNESS: return p_material.roughnessTexture;
			case Material::E_TEXTURE::AMBIENT_OCCLUSION: return p_material.ambientOcclusionTexture;
			case Material::E_TEXTURE::EMISSIVE: return p_material.emissiveTexture;
			case Material::E_TEXTURE::COUNT: break;
			}

			throw std::runtime_error( "Invalid material texture" );
		}

		const Desc::Key & _materialTextureKey( const Material::E_TEXTURE p_texture )
		{
			switch ( p_texture )
			{
			case Material::E_TEXTURE::ALBEDO: return Shading::MATERIAL_ALBEDO_TEXTURE;
			case Material::E_TEXTURE::NORMAL: return Shading::MATERIAL_NORMAL_TEXTURE;
			case Material::E_TEXTURE::METALLIC: return Shading::MATERIAL_METALLIC_TEXTURE;
			case Material::E_TEXTURE::ROUGHNESS: return Shading::MATERIAL_ROUGHNESS_TEXTURE;
			case Material::E_TEXTURE::AMBIENT_OCCLUSION: return Shading::MATERIAL_AO_TEXTURE;
			case Material::E_TEXTURE::EMISSIVE: return Shading::MATERIAL_EMISSIVE_TEXTURE;
			case Material::E_TEXTURE::COUNT: break;
			}

			throw std::runtime_error( "Invalid material texture" );
		}

		bool _isScalarTexture( const Material::E_TEXTURE p_texture )
		{
			return p_texture == Material::E_TEXTURE::METALLIC || p_texture == Material::E_TEXTURE::ROUGHNESS
				   || p_texture == Material::E_TEXTURE::AMBIENT_OCCLUSION;
		}
	} // namespace

	void Shading::loadEnvironment( Context::ContextWrapper & p_context, const ShadingConfig & p_config )
	{
		const std::vector<Baker::EnvironmentTexel> pixels = _buildCubemap( p_config );
		if ( not pixels.empty() )
		{
			p_context.setTextureData<Baker::EnvironmentTexel>(
				ENVIRONMENT_TEXTURE, std::span<const Baker::EnvironmentTexel> { pixels }
			);
		}
	}

	void Shading::loadMaterialTextures( Context::ContextWrapper & p_context, const ShadingConfig & p_config )
	{
		for ( uint8_t index = 0; index < uint8_t( Material::E_TEXTURE::COUNT ); ++index )
		{
			const Material::E_TEXTURE textureType = Material::E_TEXTURE( index );
			const auto &			  texture	  = _materialTexture( p_config.material, textureType );
			if ( not texture )
			{
				continue;
			}

			const uint8_t				 channels = _isScalarTexture( textureType ) ? 1 : 4;
			const Util::Image::ByteImage image	  = Util::Image::readByteImage( FilePath( texture->path ), channels );
			p_context.setTextureData<uint8_t>(
				_materialTextureKey( textureType ), std::span<const uint8_t> { image.pixels }, image.width, image.height
			);
		}
	}
} // namespace VTX::Renderer::Builder::PostProcess
