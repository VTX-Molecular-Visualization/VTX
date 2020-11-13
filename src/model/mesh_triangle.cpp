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

		void MeshTriangle::instantiateDefaultViews() {};

		MeshTriangle::~MeshTriangle() {}

		void MeshTriangle::_init()
		{
			_computeAABB();

			_buffer->setPositions( _vertices );
			_buffer->setNormals( _normals );
			_buffer->setColors( _colors );
			_buffer->setVisibilities( _visibilities );
			_buffer->setIndices( _indices );
		}

		void MeshTriangle::_instanciate3DViews() { _addRenderable( MVC::MvcManager::get().addViewOnModel( this, ID::View::D3_TRIANGLE, new View::D3::Triangle( this ) ) ); }

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) + " / Vertices: " + std::to_string( _vertices.size() ) + " / Normals: " + std::to_string( _normals.size() )
					  + " / Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof mesh triangle: " + std::to_string( sizeof( *this ) ) );
		}

		void MeshTriangle::_computeAABB()
		{
			_aabb.invalidate();
			for ( const Vec3f & v : _vertices )
			{
				_aabb.extend( v );
			}
		}

	} // namespace Model
} // namespace VTX
