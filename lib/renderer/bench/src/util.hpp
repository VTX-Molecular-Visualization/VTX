#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <numeric>
#include <util/constants.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Bench
{
	struct Mesh
	{
		Mat4f						   transform;
		std::vector<Vec3f>			   vertices;
		std::vector<Vec3f>			   normals;
		std::vector<Util::Color::Rgba> colors;
		std::vector<bool>			   visibilities;
		std::vector<bool>			   selections;
		std::vector<uint>			   ids;
		std::vector<uint>			   indices;
	};

	struct Molecule
	{
		Mat4f						   transform;
		std::vector<Vec3f>			   atomPositions;
		std::vector<Util::Color::Rgba> atomColors;
		std::vector<float>			   atomRadii;
		std::vector<bool>			   atomVisibilities;
		std::vector<bool>			   atomSelections;
		std::vector<uint>			   atomIds;
		std::vector<uint>			   bonds;
	};

	const Mesh DEFAULT_MESH = { MAT4F_ID,
								{ Vec3f( 0.5f, -0.5f, 0.f ), Vec3f( -0.5f, -0.5f, 0.f ), Vec3f( 0.f, 0.5f, 0.f ) },
								{ Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ) },
								{ Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ),
								  Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f ),
								  Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f ) },
								{ 1, 1, 1, 1 },
								{ 0, 0, 0, 0 },
								{ 0, 0, 0, 0 },
								{ 0, 1, 2 } };

	const Molecule DEFAULT_MOLECULE
		= { MAT4F_ID,
			{ Vec3f( -2.f, 0.f, 0.f ), Vec3f( 2.f, 0.f, 0.f ), Vec3f( 0.f, 1.f, 0.f ) },
			{ Util::Color::Rgba::random(), Util::Color::Rgba::random(), Util::Color::Rgba::random() },
			{ 0.5f, 0.3f, 0.4 },
			{ 1, 1, 1 },
			{ 0, 0, 0 },
			{ 0, 0, 0 },
			{ 0, 1, 1, 2, 2, 0 }

		  };

	Molecule generateAtomGrid( int p_size )
	{
		if ( p_size % 2 == 0 )
		{
			++p_size;
		}

		const size_t realSize = p_size * p_size * p_size;

		std::vector<Vec3f>			   positions( realSize );
		std::vector<Util::Color::Rgba> colors( realSize );
		std::vector<uint>			   bonds( ( realSize - 1 ) * 2 );

		size_t		counter = 0;
		const float offset	= 2.f;

		for ( int i = -p_size / 2; i <= p_size / 2; ++i )
		{
			for ( int j = -p_size / 2; j <= p_size / 2; ++j )
			{
				for ( int k = -p_size / 2; k <= p_size / 2; ++k )
				{
					positions[ counter ] = Vec3f( i * offset, j * offset, k * offset );
					colors[ counter ]	 = Util::Color::Rgba::random();
					if ( counter < realSize - 1 )
					{
						bonds[ counter * 2 ]	 = uint( counter );
						bonds[ counter * 2 + 1 ] = uint( counter + 1 );
					}
					++counter;
				}
			}
		}

		assert( counter == realSize );

		std::vector<uint> ids( realSize );
		std::iota( ids.begin(), ids.end(), 0 );

		VTX_INFO( "{} atoms and {} bonds generated", realSize, bonds.size() / 2 );

		return { MAT4F_ID,
				 positions,
				 colors,
				 std::vector<float>( realSize, 0.5f ),
				 std::vector<bool>( realSize, true ),
				 std::vector<bool>( realSize, false ),
				 ids,
				 bonds };
	}

	// Skybox.
	// 		const FilePath				  pathSkybox( std::filesystem::current_path() / "assets/skybox" );
	// 		const std::array<FilePath, 6> pathImages
	// 			= { pathSkybox / "right.jpg",  pathSkybox / "left.jpg",	 pathSkybox / "top.jpg",
	// 				pathSkybox / "bottom.jpg", pathSkybox / "front.jpg", pathSkybox / "back.jpg" };
	// 		std::array<unsigned char *, 6> images;
	// 		int							   width, height, nrChannels;
	// 		try
	// 		{
	// 			for ( size_t i = 0; i < pathImages.size(); ++i )
	// 			{
	// 				images[ i ] = stbi_load( pathImages[ i ].string().c_str(), &width, &height, &nrChannels, 0 );
	// 			}
	// 			renderer.loadSkybox( images, width, height );
	// 		}
	// 		catch ( const std::exception & p_e )
	// 		{
	// 			VTX_ERROR( "Skybox not found: {}", p_e.what() );
	// 		}
	// 		for ( size_t i = 0; i < images.size(); ++i )
	// 		{
	// 			stbi_image_free( images[ i ] );
	// 		}
} // namespace VTX::Bench

#endif
