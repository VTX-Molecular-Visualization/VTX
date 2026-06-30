#include "renderer/baker.hpp"
#include <array>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ktx.h>
#include <stdexcept>
#include <string>
#include <util/image.hpp>
#include <util/math.hpp>
#include <util/string.hpp>

namespace VTX::Renderer::Baker
{
	namespace
	{
		constexpr ktx_uint32_t GL_RGBA16F = 0x881A;

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

		std::vector<uint16_t> _buildCubemap( const Util::Image::FloatImage & p_image, const uint32_t p_faceSize )
		{
			if ( p_image.width != p_image.height * 2 )
			{
				throw std::runtime_error( "Environment texture must use a 2:1 equirectangular projection" );
			}
			if ( p_faceSize == 0 )
			{
				throw std::runtime_error( "Environment cubemap face size must be greater than zero" );
			}

			const size_t		  facePixelCount = size_t( p_faceSize ) * size_t( p_faceSize );
			std::vector<uint16_t> cubemap( facePixelCount * 6 * 4 );
			for ( size_t face = 0; face < 6; ++face )
			{
				for ( uint32_t y = 0; y < p_faceSize; ++y )
				{
					for ( uint32_t x = 0; x < p_faceSize; ++x )
					{
						const float	 u		= 2.f * ( float( x ) + 0.5f ) / float( p_faceSize ) - 1.f;
						const float	 v		= 2.f * ( float( y ) + 0.5f ) / float( p_faceSize ) - 1.f;
						const auto	 sample = _sampleEquirectangular( p_image, _cubemapDirection( face, u, v ) );
						const size_t offset = ( face * facePixelCount + size_t( y ) * p_faceSize + x ) * 4;
						for ( size_t channel = 0; channel < sample.size(); ++channel )
						{
							cubemap[ offset + channel ] = Util::Math::packHalf1x16( sample[ channel ] );
						}
					}
				}
			}

			return cubemap;
		}

		bool _writeCubemapToKtx(
			const FilePath &			  p_outputPath,
			const std::vector<uint16_t> & p_cubemap,
			const uint32_t				  p_faceSize
		)
		{
			ktxTextureCreateInfo textureInfo = {};
			textureInfo.glInternalformat	 = GL_RGBA16F;
			textureInfo.baseWidth			 = p_faceSize;
			textureInfo.baseHeight			 = p_faceSize;
			textureInfo.baseDepth			 = 1;
			textureInfo.numDimensions		 = 2;
			textureInfo.numLevels			 = 1;
			textureInfo.numLayers			 = 1;
			textureInfo.numFaces			 = 6;
			textureInfo.isArray				 = KTX_FALSE;
			textureInfo.generateMipmaps		 = KTX_TRUE;

			ktxTexture1 *	 texture = nullptr;
			ktx_error_code_e result	 = ktxTexture1_Create( &textureInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture );
			ktxTexture *	 baseTexture = reinterpret_cast<ktxTexture *>( texture );
			if ( result == KTX_SUCCESS )
			{
				const size_t facePixelCount = size_t( p_faceSize ) * p_faceSize;
				const size_t faceDataSize	= facePixelCount * 4 * sizeof( uint16_t );
				for ( ktx_uint32_t face = 0; face < 6 && result == KTX_SUCCESS; ++face )
				{
					const uint16_t * const faceData = p_cubemap.data() + size_t( face ) * facePixelCount * 4;
					result							= ktxTexture_SetImageFromMemory(
						baseTexture, 0, 0, face, reinterpret_cast<const ktx_uint8_t *>( faceData ), faceDataSize
					);
				}
			}

			if ( result == KTX_SUCCESS )
			{
				result = ktxTexture_WriteToNamedFile( baseTexture, p_outputPath.string().c_str() );
			}

			if ( texture != nullptr )
			{
				ktxTexture_Destroy( baseTexture );
			}

			if ( result != KTX_SUCCESS )
			{
				std::cerr << "Failed to export KTX: " << p_outputPath << " (" << ktxErrorString( result ) << ")\n";
				return false;
			}

			return true;
		}
	} // namespace

	bool isEnvironmentMapFile( const FilePath & p_path )
	{
		const std::string extension = Util::String::toLower( p_path.extension().string() );

		return extension == ".hdr" || extension == ".exr";
	}

	std::vector<uint16_t> buildEnvironmentCubemap( const FilePath & p_path, const uint32_t p_faceSize )
	{ return _buildCubemap( Util::Image::readFloatImage( p_path ), p_faceSize ); }

	bool bakeEnvironmentMapToKtx( const FilePath & p_path, const uint32_t p_faceSize )
	{
		const FilePath		  outputPath = FilePath( p_path ).replace_extension( ".ktx" );
		std::vector<uint16_t> cubemap;
		try
		{
			cubemap = buildEnvironmentCubemap( p_path, p_faceSize );
		}
		catch ( const std::exception & error )
		{
			std::cerr << error.what() << "\n";
			return false;
		}

		if ( not _writeCubemapToKtx( outputPath, cubemap, p_faceSize ) )
		{
			return false;
		}

		std::cout << "Baked " << p_path << " -> " << outputPath << "\n";
		return true;
	}

	int bakeEnvironmentDirectoryToKtx( const FilePath & p_hdrRoot, const uint32_t p_faceSize )
	{
		int failures = 0;
		int baked	 = 0;

		for ( const std::filesystem::directory_entry & entry :
			  std::filesystem::recursive_directory_iterator( p_hdrRoot ) )
		{
			if ( not entry.is_regular_file() || not isEnvironmentMapFile( entry.path() ) )
			{
				continue;
			}

			if ( bakeEnvironmentMapToKtx( entry.path(), p_faceSize ) )
			{
				++baked;
			}
			else
			{
				++failures;
			}
		}

		std::cout << "Baked " << baked << " HDR file(s)";
		if ( failures > 0 )
		{
			std::cout << ", " << failures << " failure(s)";
		}
		std::cout << ".\n";

		return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	}
} // namespace VTX::Renderer::Baker
