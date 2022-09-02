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

		void MeshTriangle::_fillBuffer()
		{
			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setColors( _colors );
			_buffer->setVisibilities( _visibilities );
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
			if ( _indices.size() < 3 )
			{
				return;
			}

			_normals.resize( _vertices.size() );
			for ( uint i = 0; i < _indices.size() - 2; i += 3 )
			{
				Vec3f normal = Util::Math::cross( _vertices[ _indices[ i + 1 ] ] - _vertices[ _indices[ i + 2 ] ],
												  _vertices[ _indices[ i + 1 ] ] - _vertices[ _indices[ i + 0 ] ] );

				assert( Util::Math::length( normal ) != 0.f );

				Util::Math::normalizeSelf( normal );

				_normals[ _indices[ i + 0 ] ] = normal;
				_normals[ _indices[ i + 1 ] ] = normal;
				_normals[ _indices[ i + 2 ] ] = normal;
			}
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
