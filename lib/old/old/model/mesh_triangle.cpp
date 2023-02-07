#include "mesh_triangle.hpp"
#include "id.hpp"
#include <util/constants.hpp>
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include "view/d3/triangle.hpp"

namespace VTX
{
	namespace Model
	{
		MeshTriangle::MeshTriangle() : BaseModel3D( VTX::ID::Model::MODEL_MESH_TRIANGLE ) {}

		void MeshTriangle::_init()
		{
			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setColors( _colors );
			_buffer->setVisibilities( _visibilities );
			_buffer->setIds( _ids );
			_buffer->setIndices( _indices );
		}

		void MeshTriangle::_computeAABB() const
		{
			_aabb.invalidate();
			for ( const Vec3f & v : _vertices )
			{
				_aabb.extend( v );
			}
		}

		void MeshTriangle::_instantiate3DViews()
		{
			_addRenderable(
				MVC::MvcManager::get().instantiateView<View::D3::Triangle>( this, VTX::ID::View::D3_TRIANGLE ) );
		}

		void MeshTriangle::recomputeNormals()
		{
			if ( _indices.size() % 3 != 0 )
			{
				assert( false );
				return;
			}

			_normals.resize( _vertices.size(), VEC4F_ZERO );
			std::vector<std::vector<Vec3f>> normals( _vertices.size(), std::vector<Vec3f>() );

			for ( uint i = 0; i < _indices.size() - 2; i += 3 )
			{
				Vec3f normal
					= Util::Math::cross( Vec3f( _vertices[ _indices[ i + 1 ] ] - _vertices[ _indices[ i + 2 ] ] ),
										 Vec3f( _vertices[ _indices[ i + 1 ] ] - _vertices[ _indices[ i + 0 ] ] ) );

				assert( Util::Math::length( normal ) != 0.f );
				Util::Math::normalizeSelf( normal );

				for ( uint j = 0; j < 3; ++j )
				{
					normals[ _indices[ i + j ] ].emplace_back( normal );
				}
			}

			for ( uint i = 0; i < normals.size(); ++i )
			{
				std::vector<Vec3f> & verticeNormals = normals[ i ];
				Vec3f				 normal			= VEC3F_ZERO;
				for ( const auto & n : verticeNormals )
				{
					normal += n;
				}
				normal /= verticeNormals.size();
				Util::Math::normalizeSelf( normal );
				_normals[ i ] = Vec4f( normal, 1.f );
			}

			_normals.shrink_to_fit();
		}

		void MeshTriangle::toIndexed()
		{
			std::vector<Vec4f> vertices = std::vector<Vec4f>();
			std::vector<uint>  indices( _indices.size() );

			for ( uint i = 0; i < _vertices.size(); ++i )
			{
				bool found = false;
				for ( uint j = 0; j < vertices.size() && found == false; ++j )
				{
					if ( glm::length2( vertices[ j ] - _vertices[ i ] ) < EPSILON * EPSILON )
					{
						indices[ i ] = j;
						found		 = true;
						break;
					}
				}
				if ( found == false )
				{
					vertices.push_back( _vertices[ i ] );
					indices[ i ] = uint( vertices.size() ) - 1;
				}
			}

			_vertices = vertices;
			_indices  = indices;
		}

		void MeshTriangle::toNonIndexed()
		{
			std::vector<Vec4f> vertices( _indices.size() );
			for ( uint i = 0; i < _indices.size(); ++i )
			{
				vertices[ i ] = _vertices[ _indices[ i ] ];
			}
			_vertices = vertices;
			std::iota( _indices.begin(), _indices.end(), 0 );
		}

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) + " / Vertices: "
					  + std::to_string( _vertices.size() ) + " / Normals: " + std::to_string( _normals.size() )
					  + " / Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof mesh triangle: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
