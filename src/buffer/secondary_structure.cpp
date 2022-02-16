#include "secondary_structure.hpp"

namespace VTX::Buffer
{
	void SecondaryStructure::generate()
	{
		_vboPositions.create();
		_vboDirections.create();
		_vboSecondaryStructures.create();
		_vboColors.create();
		_vboVisibilities.create();
		_vboSelections.create();
		_vboIds.create();
		_ibo.create();

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		// Control point position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, _vboPositions, sizeof( Vec4f ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );

		// Control point direction.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, _vboDirections, sizeof( Vec3f ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION,
								  ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );

		// Control point secondary structure.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::RESIDUE_SECONDARY_STRUCTURE );
		_vao.setVertexBuffer(
			ATTRIBUTE_LOCATION::RESIDUE_SECONDARY_STRUCTURE, _vboSecondaryStructures, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::RESIDUE_SECONDARY_STRUCTURE, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::RESIDUE_SECONDARY_STRUCTURE,
								  ATTRIBUTE_LOCATION::RESIDUE_SECONDARY_STRUCTURE );

		// Control point color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::RESIDUE_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::RESIDUE_COLOR, _vboColors, sizeof( Color::Rgb ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::RESIDUE_COLOR, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::RESIDUE_COLOR, ATTRIBUTE_LOCATION::RESIDUE_COLOR );

		// Control point visibility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::RESIDUE_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::RESIDUE_VISIBILITY, _vboVisibilities, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::RESIDUE_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::RESIDUE_VISIBILITY, ATTRIBUTE_LOCATION::RESIDUE_VISIBILITY );

		// Control point selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::RESIDUE_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::RESIDUE_SELECTION, _vboSelections, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::RESIDUE_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::RESIDUE_SELECTION, ATTRIBUTE_LOCATION::RESIDUE_SELECTION );

		// Id.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::RESIDUE_ID );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::RESIDUE_ID, _vboIds, sizeof( Model::ID ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::RESIDUE_ID, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::RESIDUE_ID, ATTRIBUTE_LOCATION::RESIDUE_ID );
	}

	void SecondaryStructure::setControlPointPositions( const std::vector<Vec4f> & p_positions )
	{
		_vboPositions.set<Vec4f>( p_positions, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setControlPointDirections( const std::vector<Vec3f> & p_directions )
	{
		_vboDirections.set<Vec3f>( p_directions, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setSecondaryStructures( const std::vector<uint> & p_ss )
	{
		_vboSecondaryStructures.set<uint>( p_ss, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setColors( const std::vector<Color::Rgb> & p_colors )
	{
		_vboColors.set<Color::Rgb>( p_colors, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setVisibilities( const std::vector<uint> & p_visibilities )
	{
		_vboVisibilities.set<uint>( p_visibilities, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setSelections( const std::vector<uint> & p_selections )
	{
		_vboSelections.set<uint>( p_selections, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setIds( const std::vector<Model::ID> & p_ids )
	{
		_vboIds.set<Model::ID>( p_ids, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void SecondaryStructure::setIndices( const std::vector<uint> & p_indices )
	{
		_ibo.set<uint>( p_indices, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

} // namespace VTX::Buffer
