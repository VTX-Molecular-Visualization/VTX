#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Buffer
{
	void SecondaryStructure::_generate()
	{
		gl()->glCreateBuffers( 1, &_vboPositions );
		gl()->glCreateBuffers( 1, &_vboDirections );
		gl()->glCreateBuffers( 1, &_vboNormals );
		gl()->glCreateBuffers( 1, &_vboSecondaryStructures );
		gl()->glCreateBuffers( 1, &_vboColors );
		gl()->glCreateBuffers( 1, &_vboSelections );
		gl()->glCreateBuffers( 1, &_ibo );

		gl()->glCreateVertexArrays( 1, &_vao );

		gl()->glVertexArrayElementBuffer( _vao, _ibo );

		// Control point position.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, _vboPositions, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );

		// Control point direction.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, _vboDirections, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );

		// Control point normal.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL, _vboNormals, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL );

		// Control point secondary structure.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, _vboSecondaryStructures, 0, sizeof( ushort ) );
		gl()->glVertexArrayAttribIFormat(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, 1, GL_UNSIGNED_SHORT, 0 );
		gl()->glVertexArrayAttribBinding( _vao,
										  ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE,
										  ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );

		// Control point color.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, _vboColors, 0, sizeof( Color::Rgb ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );

		// Control point selection.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, _vboSelections, 0, sizeof( ushort ) );
		gl()->glVertexArrayAttribIFormat( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, 1, GL_UNSIGNED_SHORT, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );
	}

	void SecondaryStructure::_free()
	{
		if ( _vao != GL_INVALID_VALUE )
		{
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );

			gl()->glDeleteVertexArrays( 1, &_vao );
		}

		if ( _vboPositions != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboPositions );
		}
		if ( _vboDirections != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboDirections );
		}
		if ( _vboNormals != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboNormals );
		}
		if ( _vboSecondaryStructures != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboSecondaryStructures );
		}
		if ( _vboColors != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboColors );
		}
		if ( _vboSelections != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboSelections );
		}
		if ( _ibo != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_ibo );
		}
	}

	void SecondaryStructure::bind() { gl()->glBindVertexArray( _vao ); }
	void SecondaryStructure::unbind() { gl()->glBindVertexArray( 0 ); }

	void SecondaryStructure::setControlPointPositions( const std::vector<Vec3f> & p_positions )
	{
		gl()->glNamedBufferData(
			_vboPositions, GLsizei( p_positions.size() ) * sizeof( Vec3f ), p_positions.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointDirections( const std::vector<Vec3f> & p_directions )
	{
		gl()->glNamedBufferData(
			_vboDirections, GLsizei( p_directions.size() ) * sizeof( Vec3f ), p_directions.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointNormals( const std::vector<Vec3f> & p_normals )
	{
		gl()->glNamedBufferData(
			_vboNormals, GLsizei( p_normals.size() ) * sizeof( Vec3f ), p_normals.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointSecondaryStructure( const std::vector<ushort> & p_ss )
	{
		gl()->glNamedBufferData(
			_vboSecondaryStructures, GLsizei( p_ss.size() ) * sizeof( ushort ), p_ss.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointColors( const std::vector<Color::Rgb> & p_colors )
	{
		gl()->glNamedBufferData(
			_vboColors, GLsizei( p_colors.size() ) * sizeof( Color::Rgb ), p_colors.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointSelections( const std::vector<ushort> & p_selections )
	{
		gl()->glNamedBufferData(
			_vboSelections, GLsizei( p_selections.size() ) * sizeof( ushort ), p_selections.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setIndices( const std::vector<uint> & p_indices )
	{
		gl()->glNamedBufferData( _ibo, GLsizei( p_indices.size() ) * sizeof( uint ), p_indices.data(), GL_STATIC_DRAW );
	}

} // namespace VTX::Buffer
