#include "mesh_triangle.hpp"
#include "manager/mvc/mvc_manager.hpp"
#include "model/id_list.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		MeshTriangle::MeshTriangle() : BaseModel3D( VTX::ID::Model::MODEL_MESH_TRIANGLE ) {}

		void MeshTriangle::_init()
		{
			// TODO VX_LOGIC Send signal when data for buffers has changed
			// New (maybe use mask ?)
			//_notifyViews(new VTXEvent(Event::ALL_INFO_CHANGED);
			// Old
			//_buffer->setPositions( _vertices );
			//_buffer->setNormals( _normals );
			//_buffer->setColors( _colors );
			//_buffer->setVisibilities( _visibilities );
			//_buffer->setIds( _ids );
			//_buffer->setIndices( _indices );
		}

		void MeshTriangle::_computeAABB() const
		{
			_aabb.invalidate();
			for ( const Vec3f & v : _vertices )
			{
				_aabb.extend( v );
			}
		}

		// void MeshTriangle::_instantiate3DViews()
		//{
		//	_addRenderable(
		//		MVC::MvcManager::get().instantiateView<View::D3::Triangle>( this, VTX::ID::View::D3_TRIANGLE ) );
		// }

		void MeshTriangle::recomputeNormals()
		{
			if ( _indices.size() % 3 != 0 )
			{
				assert( false );
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
