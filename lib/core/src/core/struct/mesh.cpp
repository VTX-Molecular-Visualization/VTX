#include "core/struct/mesh.hpp"
#include <numeric>
#include <util/math.hpp>

namespace VTX::Core::Struct
{

	void Mesh::computeNormales()
	{
		if ( indices.size() % 3 != 0 )
		{
			assert( false );
			return;
		}

		normales.resize( vertices.size(), VEC4F_ZERO );

		for ( uint i = 0; i < indices.size() - 2; i += 3 )
		{
			Vec3f normal = Util::Math::cross(
				Vec3f( vertices[ indices[ i + 1 ] ] - vertices[ indices[ i + 2 ] ] ),
				Vec3f( vertices[ indices[ i + 1 ] ] - vertices[ indices[ i + 0 ] ] )
			);

			assert( Util::Math::length( normal ) != 0.f );
			Util::Math::normalizeSelf( normal );

			for ( uint j = 0; j < 3; ++j )
			{
				normales[ indices[ i + j ] ].x += normal.x;
				normales[ indices[ i + j ] ].y += normal.y;
				normales[ indices[ i + j ] ].z += normal.z;
			}
		}

		for ( uint i = 0; i < normales.size(); ++i )
		{
			Util::Math::normalizeSelf( normales[ i ] );
		}

		normales.shrink_to_fit();
	}

	void Mesh::toIndexed()
	{
		const float		   epsilon2 = EPSILON * EPSILON;
		std::vector<Vec4f> vertices = std::vector<Vec4f>();
		std::vector<uint>  indices( indices.size() );

		for ( uint i = 0; i < vertices.size(); ++i )
		{
			bool found = false;
			for ( uint j = 0; j < vertices.size() && found == false; ++j )
			{
				if ( Util::Math::length2( vertices[ j ] - vertices[ i ] ) < epsilon2 )
				{
					indices[ i ] = j;
					found		 = true;
					break;
				}
			}
			if ( found == false )
			{
				vertices.push_back( vertices[ i ] );
				indices[ i ] = uint( vertices.size() ) - 1;
			}
		}

		vertices = vertices;
		indices	 = indices;
	}

	void Mesh::toNonIndexed()
	{
		std::vector<Vec4f> vertices( indices.size() );
		for ( uint i = 0; i < indices.size(); ++i )
		{
			vertices[ i ] = vertices[ indices[ i ] ];
		}
		vertices = vertices;
		std::iota( indices.begin(), indices.end(), 0 );
	}
} // namespace VTX::Core::Struct
