#include "core/struct/mesh.hpp"
#include <numeric>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <utility>

namespace VTX::Core::Struct
{

	void Mesh::computeNormals()
	{
		if ( indices.size() % 3 != 0 )
		{
			assert( false );
			return;
		}

		normals.assign( vertices.size(), Vec3f( 0.f ) );

		for ( size_t i = 0; i < indices.size(); i += 3 )
		{
			assert( indices[ i ] < vertices.size() );
			assert( indices[ i + 1 ] < vertices.size() );
			assert( indices[ i + 2 ] < vertices.size() );

			Vec3f normal = Util::Math::cross(
				vertices[ indices[ i + 1 ] ] - vertices[ indices[ i ] ],
				vertices[ indices[ i + 2 ] ] - vertices[ indices[ i ] ]
			);

			if ( Util::Math::length2( normal ) == 0.f )
			{
				continue;
			}
			Util::Math::normalizeSelf( normal );

			for ( size_t j = 0; j < 3; ++j )
			{
				normals[ indices[ i + j ] ] += normal;
			}
		}

		for ( Vec3f & normal : normals )
		{
			if ( Util::Math::length2( normal ) != 0.f )
			{
				Util::Math::normalizeSelf( normal );
			}
		}
	}

	void Mesh::toIndexed()
	{
		std::vector<Vec3f> indexedVertices;
		std::vector<Vec3f> indexedNormals;
		std::vector<Index> indexedIndices;
		indexedVertices.reserve( vertices.size() );
		indexedNormals.reserve( normals.size() );
		indexedIndices.reserve( vertices.size() );

		const bool hasNormals = normals.size() == vertices.size();
		for ( size_t i = 0; i < vertices.size(); ++i )
		{
			bool found = false;
			for ( size_t j = 0; j < indexedVertices.size(); ++j )
			{
				const bool samePosition = Util::Math::length2( indexedVertices[ j ] - vertices[ i ] ) < EPSILON2;
				const bool sameNormal
					= not hasNormals || Util::Math::length2( indexedNormals[ j ] - normals[ i ] ) < EPSILON2;
				if ( samePosition && sameNormal )
				{
					indexedIndices.emplace_back( static_cast<Index>( j ) );
					found = true;
					break;
				}
			}
			if ( not found )
			{
				indexedVertices.emplace_back( vertices[ i ] );
				if ( hasNormals )
				{
					indexedNormals.emplace_back( normals[ i ] );
				}
				indexedIndices.emplace_back( static_cast<Index>( indexedVertices.size() - 1 ) );
			}
		}

		vertices = std::move( indexedVertices );
		normals	 = std::move( indexedNormals );
		indices	 = std::move( indexedIndices );
	}

	void Mesh::toNonIndexed()
	{
		std::vector<Vec3f> nonIndexedVertices;
		std::vector<Vec3f> nonIndexedNormals;
		nonIndexedVertices.reserve( indices.size() );
		nonIndexedNormals.reserve( indices.size() );

		const bool hasNormals = normals.size() == vertices.size();
		for ( const Index index : indices )
		{
			assert( index < vertices.size() );
			nonIndexedVertices.emplace_back( vertices[ index ] );
			if ( hasNormals )
			{
				nonIndexedNormals.emplace_back( normals[ index ] );
			}
		}

		vertices = std::move( nonIndexedVertices );
		normals	 = std::move( nonIndexedNormals );
		indices.resize( vertices.size() );
		std::iota( indices.begin(), indices.end(), 0 );
	}
} // namespace VTX::Core::Struct
