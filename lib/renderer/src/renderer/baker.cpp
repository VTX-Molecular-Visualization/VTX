#include "renderer/baker.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <ktx.h>
#include <stdexcept>
#include <string>
#include <util/image.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/string.hpp>

namespace VTX::Renderer::Baker
{
	namespace
	{
		constexpr ktx_uint32_t VK_FORMAT_R16G16B16A16_SFLOAT = 97;
		constexpr uint		   KTX_ZSTD_COMPRESSION_LEVEL	 = 10;
		constexpr size_t	   THUMBNAIL_WIDTH				 = 256;
		constexpr size_t	   THUMBNAIL_HEIGHT				 = 128;
		constexpr size_t	   ENVIRONMENT_FACE_COUNT		 = 6;
		constexpr size_t	   ENVIRONMENT_CHANNEL_COUNT	 = 4;

		std::array<float, 4> _sampleEquirectangular(
			const Util::Image::FloatImage & p_image,
			const Vec3f &					p_direction
		)
		{
			using namespace Util::Math;

			const float u = atan2( p_direction.z, p_direction.x ) / TWO_PIf + 0.5f;
			const float v = 0.5f - asin( clamp( p_direction.y, -1.f, 1.f ) ) / PIf;

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
			using Util::Math::normalize;

			switch ( p_face )
			{
			case 0: return normalize( Vec3f( 1.f, -p_v, -p_u ) );
			case 1: return normalize( Vec3f( -1.f, -p_v, p_u ) );
			case 2: return normalize( Vec3f( p_u, 1.f, p_v ) );
			case 3: return normalize( Vec3f( p_u, -1.f, -p_v ) );
			case 4: return normalize( Vec3f( p_u, -p_v, 1.f ) );
			case 5: return normalize( Vec3f( -p_u, -p_v, -1.f ) );
			default: throw std::runtime_error( "Invalid cubemap face" );
			}
		}

		std::vector<EnvironmentTexel> _buildCubemap( const Util::Image::FloatImage & p_image, const uint p_faceSize )
		{
			if ( p_image.width != p_image.height * 2 )
			{
				throw std::runtime_error( "Environment texture must use a 2:1 equirectangular projection" );
			}
			if ( p_faceSize == 0 )
			{
				throw std::runtime_error( "Environment cubemap face size must be greater than zero" );
			}

			const size_t				  facePixelCount = size_t( p_faceSize ) * size_t( p_faceSize );
			std::vector<EnvironmentTexel> cubemap(
				facePixelCount * ENVIRONMENT_FACE_COUNT * ENVIRONMENT_CHANNEL_COUNT
			);
			for ( size_t face = 0; face < ENVIRONMENT_FACE_COUNT; ++face )
			{
				for ( uint y = 0; y < p_faceSize; ++y )
				{
					for ( uint x = 0; x < p_faceSize; ++x )
					{
						const float	 u		= 2.f * ( float( x ) + 0.5f ) / float( p_faceSize ) - 1.f;
						const float	 v		= 2.f * ( float( y ) + 0.5f ) / float( p_faceSize ) - 1.f;
						const auto	 sample = _sampleEquirectangular( p_image, _cubemapDirection( face, u, v ) );
						const size_t offset
							= ( face * facePixelCount + size_t( y ) * p_faceSize + x ) * ENVIRONMENT_CHANNEL_COUNT;
						for ( size_t channel = 0; channel < ENVIRONMENT_CHANNEL_COUNT; ++channel )
						{
							cubemap[ offset + channel ] = Util::Math::packHalf1x16( sample[ channel ] );
						}
					}
				}
			}

			return cubemap;
		}

		uint8_t _toThumbnailChannel( const float p_value )
		{
			using namespace Util::Math;

			const float mapped = p_value / ( p_value + 1.f );
			return uint8_t( clamp( std::pow( mapped, 1.f / 2.2f ) * 255.f, 0.f, 255.f ) );
		}

		bool _writeThumbnail( const FilePath & p_outputPath, const Util::Image::FloatImage & p_image )
		{
			std::vector<std::byte> pixels( THUMBNAIL_WIDTH * THUMBNAIL_HEIGHT * ENVIRONMENT_CHANNEL_COUNT );

			for ( size_t y = 0; y < THUMBNAIL_HEIGHT; ++y )
			{
				for ( size_t x = 0; x < THUMBNAIL_WIDTH; ++x )
				{
					const size_t sourceX = x * p_image.width / THUMBNAIL_WIDTH;
					const size_t sourceY = y * p_image.height / THUMBNAIL_HEIGHT;
					const size_t src	 = ( sourceY * p_image.width + sourceX ) * ENVIRONMENT_CHANNEL_COUNT;
					const size_t dstY	 = THUMBNAIL_HEIGHT - 1 - y;
					const size_t dst	 = ( dstY * THUMBNAIL_WIDTH + x ) * ENVIRONMENT_CHANNEL_COUNT;

					pixels[ dst + 0 ] = std::byte { _toThumbnailChannel( p_image.pixels[ src + 0 ] ) };
					pixels[ dst + 1 ] = std::byte { _toThumbnailChannel( p_image.pixels[ src + 1 ] ) };
					pixels[ dst + 2 ] = std::byte { _toThumbnailChannel( p_image.pixels[ src + 2 ] ) };
					pixels[ dst + 3 ] = std::byte { 255 };
				}
			}

			try
			{
				Util::Image::write(
					FilePath( p_outputPath ).replace_extension( ".png" ),
					Util::Image::E_FORMAT::PNG,
					THUMBNAIL_WIDTH,
					THUMBNAIL_HEIGHT,
					pixels.data()
				);
			}
			catch ( const std::exception & error )
			{
				VTX_ERROR( "Failed to export environment thumbnail: {}", error.what() );
				return false;
			}

			return true;
		}

		bool _writeCubemapToKtx(
			const FilePath &					  p_outputPath,
			const std::vector<EnvironmentTexel> & p_cubemap,
			const uint							  p_faceSize
		)
		{
			ktxTextureCreateInfo textureInfo = {};
			textureInfo.vkFormat			 = VK_FORMAT_R16G16B16A16_SFLOAT;
			textureInfo.baseWidth			 = p_faceSize;
			textureInfo.baseHeight			 = p_faceSize;
			textureInfo.baseDepth			 = 1;
			textureInfo.numDimensions		 = 2;
			textureInfo.numLevels			 = 1;
			textureInfo.numLayers			 = 1;
			textureInfo.numFaces			 = ENVIRONMENT_FACE_COUNT;
			textureInfo.isArray				 = KTX_FALSE;
			textureInfo.generateMipmaps		 = KTX_TRUE;

			ktxTexture2 *	 texture = nullptr;
			ktx_error_code_e result	 = ktxTexture2_Create( &textureInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture );
			ktxTexture *	 baseTexture = ktxTexture( texture );
			if ( result == KTX_SUCCESS )
			{
				result = ktxTexture2_SetTransferFunction( texture, KHR_DF_TRANSFER_LINEAR );
			}
			if ( result == KTX_SUCCESS )
			{
				result = ktxTexture2_SetPrimaries( texture, KHR_DF_PRIMARIES_BT709 );
			}
			if ( result == KTX_SUCCESS )
			{
				const size_t facePixelCount = size_t( p_faceSize ) * p_faceSize;
				const size_t faceDataSize	= facePixelCount * ENVIRONMENT_CHANNEL_COUNT * sizeof( EnvironmentTexel );
				for ( ktx_uint32_t face = 0; face < ENVIRONMENT_FACE_COUNT && result == KTX_SUCCESS; ++face )
				{
					const EnvironmentTexel * const faceData
						= p_cubemap.data() + size_t( face ) * facePixelCount * ENVIRONMENT_CHANNEL_COUNT;
					result = ktxTexture_SetImageFromMemory(
						baseTexture, 0, 0, face, reinterpret_cast<const ktx_uint8_t *>( faceData ), faceDataSize
					);
				}
			}

			if ( result == KTX_SUCCESS )
			{
				result = ktxTexture2_DeflateZstd( texture, KTX_ZSTD_COMPRESSION_LEVEL );
			}

			if ( result == KTX_SUCCESS )
			{
				result = ktxTexture2_WriteToNamedFile( texture, p_outputPath.string().c_str() );
			}

			if ( texture != nullptr )
			{
				ktxTexture2_Destroy( texture );
			}

			if ( result != KTX_SUCCESS )
			{
				VTX_ERROR( "Failed to export KTX: {} ({})", p_outputPath.string(), ktxErrorString( result ) );
				return false;
			}

			return true;
		}

		std::vector<EnvironmentTexel> _loadCubemapKtx( const FilePath & p_path, const uint p_faceSize )
		{
			ktxTexture2 *	 texture = nullptr;
			ktx_error_code_e result	 = ktxTexture2_CreateFromNamedFile(
				p_path.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture
			);
			if ( result != KTX_SUCCESS )
			{
				throw std::runtime_error(
					"Unable to load KTX '" + p_path.string() + "': " + std::string( ktxErrorString( result ) )
				);
			}

			if ( texture->vkFormat != VK_FORMAT_R16G16B16A16_SFLOAT || texture->baseWidth != p_faceSize
				 || texture->baseHeight != p_faceSize || texture->numFaces != ENVIRONMENT_FACE_COUNT )
			{
				ktxTexture2_Destroy( texture );
				throw std::runtime_error( "Invalid environment KTX texture: '" + p_path.string() + "'" );
			}

			const size_t facePixelCount = size_t( p_faceSize ) * p_faceSize;
			const size_t faceDataSize	= facePixelCount * ENVIRONMENT_CHANNEL_COUNT * sizeof( EnvironmentTexel );
			std::vector<EnvironmentTexel> cubemap(
				facePixelCount * ENVIRONMENT_FACE_COUNT * ENVIRONMENT_CHANNEL_COUNT
			);

			ktxTexture * const	baseTexture = ktxTexture( texture );
			ktx_uint8_t * const data		= ktxTexture_GetData( baseTexture );
			for ( ktx_uint32_t face = 0; face < ENVIRONMENT_FACE_COUNT; ++face )
			{
				ktx_size_t offset = 0;
				result			  = ktxTexture2_GetImageOffset( texture, 0, 0, face, &offset );
				if ( result != KTX_SUCCESS )
				{
					ktxTexture2_Destroy( texture );
					throw std::runtime_error(
						"Unable to read KTX face offset '" + p_path.string()
						+ "': " + std::string( ktxErrorString( result ) )
					);
				}

				std::memcpy(
					cubemap.data() + size_t( face ) * facePixelCount * ENVIRONMENT_CHANNEL_COUNT,
					data + offset,
					faceDataSize
				);
			}

			ktxTexture2_Destroy( texture );
			return cubemap;
		}
	} // namespace

	bool isEnvironmentMapFile( const FilePath & p_path )
	{
		const std::string extension = Util::String::toLower( p_path.extension().string() );

		return extension == ".hdr" || extension == ".exr";
	}

	std::vector<EnvironmentTexel> buildEnvironmentCubemap( const FilePath & p_path, const uint p_faceSize )
	{ return _buildCubemap( Util::Image::readFloatImage( p_path ), p_faceSize ); }

	std::vector<EnvironmentTexel> loadEnvironmentCubemapKtx( const FilePath & p_path, const uint p_faceSize )
	{ return _loadCubemapKtx( p_path, p_faceSize ); }

	bool bakeEnvironmentMapToKtx( const FilePath & p_path, const FilePath & p_outputPath, const uint p_faceSize )
	{
		Util::Image::FloatImage		  image;
		std::vector<EnvironmentTexel> cubemap;
		try
		{
			image	= Util::Image::readFloatImage( p_path );
			cubemap = _buildCubemap( image, p_faceSize );
		}
		catch ( const std::exception & error )
		{
			VTX_ERROR( "{}", error.what() );
			return false;
		}

		if ( p_outputPath.has_parent_path() )
		{
			std::filesystem::create_directories( p_outputPath.parent_path() );
		}

		if ( not _writeCubemapToKtx( p_outputPath, cubemap, p_faceSize ) )
		{
			return false;
		}

		if ( not _writeThumbnail( p_outputPath, image ) )
		{
			return false;
		}

		VTX_INFO( "Baked {} -> {}", p_path.string(), p_outputPath.string() );
		return true;
	}

} // namespace VTX::Renderer::Baker
