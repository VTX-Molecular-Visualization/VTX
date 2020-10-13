#include "mesh_triangle.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/d3/triangle.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		MeshTriangle::MeshTriangle() {}

		void MeshTriangle::instantiateDefaultViews() { MVC::MvcManager::get().addViewOnModel( this, ID::View::D3_TRIANGLE, new View::D3::Triangle( this ) ); };

		MeshTriangle::~MeshTriangle()
		{
			if ( _vao != GL_INVALID_VALUE )
			{
				OGL().glBindVertexArray( _vao );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
				OGL().glBindVertexArray( 0 );

				if ( _vboPositions != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_vboPositions );
				if ( _vboNormals != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_vboNormals );
				if ( _vboColors != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_vboColors );
				if ( _vboVisibilities != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_vboVisibilities );
				if ( _ibo != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_ibo );

				OGL().glDeleteVertexArrays( 1, &_vao );
			}
		}

		void MeshTriangle::init()
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().makeCurrent();

			_computeAABB();

			// VBO.
			OGL().glGenBuffers( 1, &_vboPositions );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			OGL().glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof( Vec3f ), _vertices.data(), GL_STATIC_DRAW );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glGenBuffers( 1, &_vboNormals );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
			OGL().glBufferData( GL_ARRAY_BUFFER, _normals.size() * sizeof( Vec3f ), _normals.data(), GL_STATIC_DRAW );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glGenBuffers( 1, &_vboColors );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			OGL().glBufferData( GL_ARRAY_BUFFER, _colors.size() * sizeof( Color::Rgb ), _colors.data(), GL_STATIC_DRAW );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glGenBuffers( 1, &_vboVisibilities );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			OGL().glBufferData( GL_ARRAY_BUFFER, _visibilities.size() * sizeof( uint ), _visibilities.data(), GL_STATIC_DRAW );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// IBO.
			OGL().glGenBuffers( 1, &_ibo );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
			OGL().glBufferData( GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof( uint32_t ), _indices.data(), GL_STATIC_DRAW );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// VAO.
			OGL().glGenVertexArrays( 1, &_vao );
			OGL().glBindVertexArray( _vao );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof( uint ), 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			OGL().glBindVertexArray( 0 );

			VTXApp::get().getMainWindow().getOpenGLWidget().doneCurrent();
		}

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) + " / Vertices: " + std::to_string( _vertices.size() ) + " / Normals: " + std::to_string( _normals.size() )
					  + " / Indices: " + std::to_string( _indices.size() ) );

			VTX_DEBUG( "Sizeof mesh triangle: " + std::to_string( sizeof( *this ) ) );
		}

		void MeshTriangle::bindBuffers()
		{
			OGL().glBindVertexArray( _vao );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
		}

		void MeshTriangle::unbindBuffers()
		{
			OGL().glBindVertexArray( 0 );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
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
