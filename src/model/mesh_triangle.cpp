#include "mesh_triangle.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include "view/d3/triangle.hpp"

namespace VTX
{
	namespace Model
	{
		MeshTriangle::MeshTriangle() : BaseModel3D( ID::Model::MODEL_MESH_TRIANGLE ) {}

		void MeshTriangle::_init() {}

		void MeshTriangle::_fillBuffer()
		{
			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setColors( _colors );
			_buffer->setVisibilities( _visibilities );
			_buffer->setIndices( _indices );
		}

		void MeshTriangle::_computeAABB()
		{
			_aabb.invalidate();
			for ( const Vec3f & v : _vertices )
			{
				_aabb.extend( v );
			}
		}

		void MeshTriangle::_instanciate3DViews() { _addRenderable( MVC::MvcManager::get().instanciateView<View::D3::Triangle>( this, ID::View::D3_TRIANGLE ) ); }

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) + " / Vertices: " + std::to_string( _vertices.size() ) + " / Normals: " + std::to_string( _normals.size() )
					  + " / Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof mesh triangle: " + std::to_string( sizeof( *this ) ) );
		}

	} // namespace Model
} // namespace VTX
