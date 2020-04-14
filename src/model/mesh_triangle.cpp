#include "mesh_triangle.hpp"
#include "util/logger.hpp"
#include "view/d3/triangle.hpp"

namespace VTX
{
	namespace Model
	{
		MeshTriangle::~MeshTriangle()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
			// glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );

			if ( _vboPositions != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_vboPositions );
			if ( _ibo != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_ibo );
			if ( _vao != GL_INVALID_VALUE ) glDeleteVertexArrays( 1, &_vao );
		}

		void MeshTriangle::init()
		{
			BaseModel::init();

			// VBO.
			glGenBuffers( 1, &_vboPositions );
			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glBufferData( GL_ARRAY_BUFFER, _vertices.size() * sizeof( Vec3f ), _vertices.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// IBO.
			glGenBuffers( 1, &_ibo );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof( uint32_t ), _indices.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// VAO.
			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );

			glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );

			// glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );

			/*
			glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_COLOR, // Id attribute
								   3,								 // # component
								   GL_FLOAT,						 // type component
								   GL_FALSE,						 // not normalized
								   sizeof( Vertex2DColor ),			 // size vertex
								   (void *)offsetof( Vertex2DColor, color ) );
								   */

			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		void MeshTriangle::_addItems()
		{
			// Add views.
			addItem( (View::BaseView<BaseModel> *)Generic::create<MeshTriangle, View::D3::Triangle>( this ) );
		}

		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _indices.size() / 3 ) );
			VTX_INFO( "Vertices: " + std::to_string( _vertices.size() ) );
		}

		void MeshTriangle::bindBuffers()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
		}

		void MeshTriangle::unbindBuffers()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

	} // namespace Model
} // namespace VTX
