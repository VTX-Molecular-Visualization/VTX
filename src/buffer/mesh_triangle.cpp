#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Buffer
{
	void MeshTriangle::_generate()
	{
		gl()->glCreateBuffers( 1, &_vboPositions );
		gl()->glCreateBuffers( 1, &_vboNormals );
		gl()->glCreateBuffers( 1, &_vboColors );
		gl()->glCreateBuffers( 1, &_vboVisibilities );
		gl()->glCreateBuffers( 1, &_ibo );

		gl()->glCreateVertexArrays( 1, &_vao );

		gl()->glVertexArrayElementBuffer( _vao, _ibo );

		// Position.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_POSITION );
		gl()->glVertexArrayVertexBuffer( _vao, ATTRIBUTE_LOCATION::VERTEX_POSITION, _vboPositions, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::VERTEX_POSITION, ATTRIBUTE_LOCATION::VERTEX_POSITION );

		// Color.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_NORMAL );
		gl()->glVertexArrayVertexBuffer( _vao, ATTRIBUTE_LOCATION::VERTEX_NORMAL, _vboNormals, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _vao, ATTRIBUTE_LOCATION::VERTEX_NORMAL, ATTRIBUTE_LOCATION::VERTEX_NORMAL );

		// Radius.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_COLOR );
		gl()->glVertexArrayVertexBuffer( _vao, ATTRIBUTE_LOCATION::VERTEX_COLOR, _vboColors, 0, sizeof( Color::Rgb ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::VERTEX_COLOR, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _vao, ATTRIBUTE_LOCATION::VERTEX_COLOR, ATTRIBUTE_LOCATION::VERTEX_COLOR );

		// Visbility.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, _vboVisibilities, 0, sizeof( ushort ) );
		gl()->glVertexArrayAttribIFormat( _vao, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, 1, GL_UNSIGNED_SHORT, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
	}

	void MeshTriangle::_free()
	{
		if ( _vao != GL_INVALID_VALUE )
		{
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_POSITION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_NORMAL );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_COLOR );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );

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

	void MeshTriangle::bind() { gl()->glBindVertexArray( _vao ); }
	void MeshTriangle::unbind() { gl()->glBindVertexArray( 0 ); }

	void MeshTriangle::setPositions( const std::vector<Vec3f> & p_positions )
	{
		gl()->glNamedBufferData(
			_vboPositions, sizeof( Vec3f ) * GLsizei( p_positions.size() ), p_positions.data(), GL_STATIC_DRAW );
	}

	void MeshTriangle::setNormals( const std::vector<Vec3f> & p_normals )
	{
		gl()->glNamedBufferData(
			_vboNormals, sizeof( Vec3f ) * GLsizei( p_normals.size() ), p_normals.data(), GL_STATIC_DRAW );
	}

	void MeshTriangle::setColors( const std::vector<Color::Rgb> & p_colors )
	{
		gl()->glNamedBufferData(
			_vboColors, sizeof( Color::Rgb ) * GLsizei( p_colors.size() ), p_colors.data(), GL_STATIC_DRAW );
	}

	void MeshTriangle::setVisibilities( const std::vector<ushort> & p_visibilities )
	{
		gl()->glNamedBufferData( _vboVisibilities,
								 sizeof( ushort ) * GLsizei( p_visibilities.size() ),
								 p_visibilities.data(),
								 GL_STATIC_DRAW );
	}

	void MeshTriangle::setIndices( const std::vector<uint> & p_indices )
	{
		gl()->glNamedBufferData( _ibo, sizeof( uint ) * GLsizei( p_indices.size() ), p_indices.data(), GL_STATIC_DRAW );
	}

} // namespace VTX::Buffer
