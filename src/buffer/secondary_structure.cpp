#include "secondary_structure.hpp"

namespace VTX::Buffer
{
	void SecondaryStructure::_generate()
	{
		gl()->glCreateBuffers( 1, &_vboPositions );
		gl()->glCreateBuffers( 1, &_vboDirections );
		gl()->glCreateBuffers( 1, &_vboSecondaryStructures );
		gl()->glCreateBuffers( 1, &_vboColors );
		gl()->glCreateBuffers( 1, &_vboSelections );
		gl()->glCreateBuffers( 1, &_ibo );

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		// Control point position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, _vboPositions, sizeof( Vec4f ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );

		// Control point direction.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, _vboDirections, sizeof( Vec3f ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );

		// Control point secondary structure.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
		_vao.setVertexBuffer(
			ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, _vboSecondaryStructures, sizeof( ushort ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, 1, Renderer::GL::VertexArray::Type::UNSIGNED_SHORT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );

		// Control point color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, _vboColors, sizeof( Color::Rgb ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );

		// Control point selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, _vboSelections, sizeof( ushort ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_SHORT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );
	}

	void SecondaryStructure::_free()
	{
		/*if ( _vao != GL_INVALID_VALUE )
		{
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );

			gl()->glDeleteVertexArrays( 1, &_vao );
		}*/

		if ( _vboPositions != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboPositions );
		}
		if ( _vboDirections != GL_INVALID_VALUE )
		{
			gl()->glDeleteBuffers( 1, &_vboDirections );
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

	void SecondaryStructure::bind() { _vao.bind(); }
	void SecondaryStructure::unbind() { _vao.unbind(); }

	void SecondaryStructure::setControlPointPositions( const std::vector<Vec4f> & p_positions )
	{
		gl()->glNamedBufferData(
			_vboPositions, GLsizei( p_positions.size() ) * sizeof( Vec4f ), p_positions.data(), GL_STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointDirections( const std::vector<Vec3f> & p_directions )
	{
		gl()->glNamedBufferData(
			_vboDirections, GLsizei( p_directions.size() ) * sizeof( Vec3f ), p_directions.data(), GL_STATIC_DRAW );
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
