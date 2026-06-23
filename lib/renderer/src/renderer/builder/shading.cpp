#include "renderer/builder/post_process/shading.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <span>
#include <stdexcept>
#include <util/image.hpp>
#include <util/math.hpp>

namespace VTX::Renderer::Builder::PostProcess
{
	namespace
	{

		std::array<float, 4> _sampleEquirectangular(
			const Util::Image::FloatImage & p_image,
			const Vec3f &					p_direction
		)
		{
			using namespace VTX::Util::Math;

			constexpr float pi = PIf;
			const float		u  = atan2( p_direction.z, p_direction.x ) / ( 2.f * pi ) + 0.5f;
			const float		v  = 0.5f - asin( clamp( p_direction.y, -1.f, 1.f ) ) / pi;

			const float x  = u * float( p_image.width ) - 0.5f;
			const float y  = v * float( p_image.height ) - 0.5f;
			const int	x0 = static_cast<int>( floor( x ) );
			const int	y0 = static_cast<int>( floor( y ) );
			const float tx = x - float( x0 );
			const float ty = y - float( y0 );

			auto pixel = [ & ]( const int p_x, const int p_y, const size_t p_channel )
			{
				const int wrappedX = ( p_x % int( p_image.width ) + int( p_image.width ) ) % int( p_image.width );
				const int clampedY = clamp( p_y, 0, int( p_image.height ) - 1 );
				return p_image.pixels[ ( size_t( clampedY ) * p_image.width + size_t( wrappedX ) ) * 4 + p_channel ];
			};

			std::array<float, 4> result;
			for ( size_t channel = 0; channel < result.size(); ++channel )
			{
				const float top	   = lerp( pixel( x0, y0, channel ), pixel( x0 + 1, y0, channel ), tx );
				const float bottom = lerp( pixel( x0, y0 + 1, channel ), pixel( x0 + 1, y0 + 1, channel ), tx );
				result[ channel ]  = lerp( top, bottom, ty );
			}
			return result;
		}

		Vec3f _cubemapDirection( const size_t p_face, const float p_u, const float p_v )
		{
			switch ( p_face )
			{
			case 0: return glm::normalize( Vec3f( 1.f, -p_v, -p_u ) );
			case 1: return glm::normalize( Vec3f( -1.f, -p_v, p_u ) );
			case 2: return glm::normalize( Vec3f( p_u, 1.f, p_v ) );
			case 3: return glm::normalize( Vec3f( p_u, -1.f, -p_v ) );
			case 4: return glm::normalize( Vec3f( p_u, -p_v, 1.f ) );
			case 5: return glm::normalize( Vec3f( -p_u, -p_v, -1.f ) );
			default: throw std::runtime_error( "Invalid cubemap face" );
			}
		}

		std::vector<uint16_t> _buildCubemap( const ShadingConfig & p_config )
		{
			if ( not p_config.environmentPath )
			{
				return {};
			}

			const Util::Image::FloatImage image = Util::Image::readFloatImage( *p_config.environmentPath );
			if ( image.width != image.height * 2 )
			{
				throw std::runtime_error( "Environment texture must use a 2:1 equirectangular projection" );
			}
			if ( p_config.environmentFaceSize == 0 )
			{
				throw std::runtime_error( "Environment cubemap face size must be greater than zero" );
			}

			const size_t facePixelCount
				= size_t( p_config.environmentFaceSize ) * size_t( p_config.environmentFaceSize );
			std::vector<uint16_t> cubemap( facePixelCount * 6 * 4 );
			for ( size_t face = 0; face < 6; ++face )
			{
				for ( uint32_t y = 0; y < p_config.environmentFaceSize; ++y )
				{
					for ( uint32_t x = 0; x < p_config.environmentFaceSize; ++x )
					{
						const float	 u		= 2.f * ( float( x ) + 0.5f ) / float( p_config.environmentFaceSize ) - 1.f;
						const float	 v		= 2.f * ( float( y ) + 0.5f ) / float( p_config.environmentFaceSize ) - 1.f;
						const auto	 sample = _sampleEquirectangular( image, _cubemapDirection( face, u, v ) );
						const size_t offset
							= ( face * facePixelCount + size_t( y ) * p_config.environmentFaceSize + x ) * 4;
						for ( size_t channel = 0; channel < sample.size(); ++channel )
						{
							cubemap[ offset + channel ] = Util::Math::packHalf1x16( sample[ channel ] );
						}
					}
				}
			}

			return cubemap;
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
		const std::vector<uint16_t> pixels = _buildCubemap( p_config );
		if ( not pixels.empty() )
		{
			p_context.setTextureData<uint16_t>( ENVIRONMENT_TEXTURE, std::span<const uint16_t> { pixels } );
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
