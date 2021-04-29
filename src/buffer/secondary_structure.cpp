#include "secondary_structure.hpp"

namespace VTX::Buffer
{
	void SecondaryStructure::_generate()
	{
		_vboPositions.create();
		_vboDirections.create();
		_vboSecondaryStructures.create();
		_vboColors.create();
		_vboSelections.create();
		_ibo.create();

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
			ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, _vboSecondaryStructures, sizeof( uint ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
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
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, _vboSelections, sizeof( uint ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_SELECTION );
	}

	void SecondaryStructure::bind() { _vao.bind(); }
	void SecondaryStructure::unbind() { _vao.unbind(); }

	void SecondaryStructure::setControlPointPositions( const std::vector<Vec4f> & p_positions )
	{
		_vboPositions.set<Vec4f>( p_positions, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointDirections( const std::vector<Vec3f> & p_directions )
	{
		_vboDirections.set<Vec3f>( p_directions, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointSecondaryStructure( const std::vector<uint> & p_ss )
	{
		_vboSecondaryStructures.set<uint>( p_ss, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointColors( const std::vector<Color::Rgb> & p_colors )
	{
		_vboColors.set<Color::Rgb>( p_colors, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointSelections( const std::vector<uint> & p_selections )
	{
		_vboSelections.set<uint>( p_selections, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setIndices( const std::vector<uint> & p_indices )
	{
		_ibo.set<uint>( p_indices, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

} // namespace VTX::Buffer
