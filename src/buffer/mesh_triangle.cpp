#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Buffer
	{
		void MeshTriangle::_generate()
		{
			gl()->glGenBuffers( 1, &_vboPositions );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboNormals );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboColors );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboVisibilities );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_ibo );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			gl()->glGenVertexArrays( 1, &_vao );
			gl()->glBindVertexArray( _vao );

			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::VERTEX_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
			gl()->glVertexAttribIPointer( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, 1, GL_UNSIGNED_SHORT, sizeof( ushort ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			gl()->glBindVertexArray( 0 );
		}

		void MeshTriangle::_free()
		{
			if ( _vao != GL_INVALID_VALUE )
			{
				gl()->glBindVertexArray( _vao );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_POSITION );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_COLOR );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboVisibilities );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
				gl()->glBindVertexArray( 0 );

				gl()->glDeleteVertexArrays( 1, &_vao );
			}

			if ( _vboPositions != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboPositions );
			}
			if ( _vboNormals != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboNormals );
			}
			if ( _vboColors != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboColors );
			}
			if ( _vboVisibilities != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboVisibilities );
			}
			if ( _ibo != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_ibo );
			}
		}

		void MeshTriangle::bind()
		{
			gl()->glBindVertexArray( _vao );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
		}
		void MeshTriangle::unbind()
		{
			gl()->glBindVertexArray( 0 );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void MeshTriangle::setPositions( const std::vector<Vec3f> & p_positions )
		{
			gl()->glNamedBufferData( _vboPositions, sizeof( Vec3f ) * GLsizei( p_positions.size() ), p_positions.data(), GL_STATIC_DRAW );
		}

		void MeshTriangle::setNormals( const std::vector<Vec3f> & p_normals )
		{
			gl()->glNamedBufferData( _vboNormals, sizeof( Vec3f ) * GLsizei( p_normals.size() ), p_normals.data(), GL_STATIC_DRAW );
		}

		void MeshTriangle::setColors( const std::vector<Color::Rgb> & p_colors )
		{
			gl()->glNamedBufferData( _vboColors, sizeof( Color::Rgb ) * GLsizei( p_colors.size() ), p_colors.data(), GL_STATIC_DRAW );
		}

		void MeshTriangle::setVisibilities( const std::vector<ushort> & p_visibilities )
		{
			gl()->glNamedBufferData( _vboVisibilities, sizeof( ushort ) * GLsizei( p_visibilities.size() ), p_visibilities.data(), GL_STATIC_DRAW );
		}

		void MeshTriangle::setIndices( const std::vector<uint> & p_indices )
		{
			gl()->glNamedBufferData( _ibo, sizeof( uint ) * GLsizei( p_indices.size() ), p_indices.data(), GL_STATIC_DRAW );
		}

	} // namespace Buffer
} // namespace VTX
