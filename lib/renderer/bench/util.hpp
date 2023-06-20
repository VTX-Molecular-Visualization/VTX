#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <core/struct_mesh.hpp>
#include <core/struct_molecule.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Bench
{
	const Core::StructMesh DEFAULT_MESH
		= { MAT4F_ID,
			{ Vec3f( 0.5f, -0.5f, 0.f ), Vec3f( -0.5f, -0.5f, 0.f ), Vec3f( 0.f, 0.5f, 0.f ) },
			{ Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ) },
			{ Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ),
			  Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f ),
			  Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f ) },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 1, 2 } };

	const Core::StructMolecule DEFAULT_MOLECULE
		= { MAT4F_ID,
			{ Vec3f( -2.f, 0.f, 0.f ), Vec3f( 2.f, 0.f, 0.f ), Vec3f( 0.f, 1.f, 0.f ) },
			{ Util::Color::Rgba::random(), Util::Color::Rgba::random(), Util::Color::Rgba::random() },
			{ 0.5f, 0.3f, 0.4 },
			{ 1, 1, 1 },
			{ 0, 0, 0 },
			{ 0, 0, 0 },
			{ 0, 1, 1, 2, 2, 0 }

		  };

	Core::StructMolecule generateAtomGrid( int p_size )
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
		const float offset	= 10.f;

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

		VTX_INFO( "{} atoms and {} bonds generated", realSize, bonds.size() / 2 );

		return { MAT4F_ID,
				 positions,
				 colors,
				 std::vector<float>( realSize, 0.5f ),
				 std::vector<uint>( realSize, 1 ),
				 std::vector<uint>( realSize, 0 ),
				 std::vector<uint>( realSize, 0 ),
				 bonds };
	}
} // namespace VTX::Bench

#endif
