#include "mesh_triangle.hpp"
#include "id.hpp"
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

		void MeshTriangle::computeNormals( const std::vector<Vec4f> & p_vertices,
										   const std::vector<uint> &  p_indices,
										   std::vector<Vec4f> &		  p_normals )
		{
			if ( p_indices.size() % 3 != 0 )
			{
				assert( false );
				return;
			}

			p_normals.resize( p_vertices.size(), VEC4F_ZERO );
			std::vector<std::vector<Vec3f>> normals( p_vertices.size(), std::vector<Vec3f>() );

			for ( uint i = 0; i < p_indices.size() - 2; i += 3 )
			{
				Vec3f normal
					= Util::Math::cross( Vec3f( p_vertices[ p_indices[ i + 1 ] ] - p_vertices[ p_indices[ i + 2 ] ] ),
										 Vec3f( p_vertices[ p_indices[ i + 1 ] ] - p_vertices[ p_indices[ i + 0 ] ] ) );

				assert( Util::Math::length( normal ) != 0.f );
				Util::Math::normalizeSelf( normal );

				for ( uint j = 0; j < 3; ++j )
				{
					normals[ p_indices[ i + j ] ].emplace_back( normal );
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
				p_normals[ i ] = Vec4f( normal, 1.f );
			}

			p_normals.shrink_to_fit();
		}

		void MeshTriangle::toIndexed( std::vector<Vec4f> & p_vertices, std::vector<uint> & p_indices )
		{
			std::vector<Vec4f> vertices = std::vector<Vec4f>();
			std::vector<uint>  indices( p_indices.size() );

			for ( uint i = 0; i < p_vertices.size(); ++i )
			{
				bool found = false;
				for ( uint j = 0; j < vertices.size() && found == false; ++j )
				{
					if ( Util::Math::length2( vertices[ j ] - p_vertices[ i ] ) < EPSILON * EPSILON )
					{
						indices[ i ] = j;
						found		 = true;
						break;
					}
				}
				if ( found == false )
				{
					vertices.push_back( p_vertices[ i ] );
					indices[ i ] = uint( vertices.size() ) - 1;
				}
			}

			p_vertices = vertices;
			p_indices  = indices;
		}

		void MeshTriangle::toNonIndexed( std::vector<Vec4f> & p_vertices, std::vector<uint> & p_indices )
		{
			std::vector<Vec4f> vertices( p_indices.size() );
			for ( uint i = 0; i < p_indices.size(); ++i )
			{
				vertices[ i ] = p_vertices[ p_indices[ i ] ];
			}
			p_vertices = vertices;
			std::iota( p_indices.begin(), p_indices.end(), 0 );
		}

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) + " / Vertices: "
					  + std::to_string( _vertices.size() ) + " / Normals: " + std::to_string( _normals.size() )
					  + " / Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof mesh triangle: " + std::to_string( sizeof( *this ) ) );
		}

		void MeshTriangle::toIndexed() {}

	} // namespace Model
} // namespace VTX
