#include "mesh_triangle.hpp"

namespace VTX::Buffer
{
	void MeshTriangle::_generate()
	{
		_vboPositions.create();
		_vboNormals.create();
		_vboColors.create();
		_vboVisibilities.create();
		_vboSelections.create();
		_vboIds.create();
		_ibo.create();
		_bufferAtomsToTriangles.create();

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		// Position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_POSITION, _vboPositions, sizeof( Vec4f ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_POSITION, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_POSITION, ATTRIBUTE_LOCATION::VERTEX_POSITION );

		// Normal.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_NORMAL, _vboNormals, sizeof( Vec4f ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_NORMAL, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_NORMAL, ATTRIBUTE_LOCATION::VERTEX_NORMAL );

		// Color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_COLOR, _vboColors, sizeof( Color::Rgba ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_COLOR, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_COLOR, ATTRIBUTE_LOCATION::VERTEX_COLOR );

		// Visbility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, _vboVisibilities, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );

		// Selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_SELECTION, _vboSelections, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::VERTEX_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_SELECTION, ATTRIBUTE_LOCATION::VERTEX_SELECTION );

		// Id.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_ID );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_ID, _vboIds, sizeof( uint ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_ID, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_ID, ATTRIBUTE_LOCATION::VERTEX_ID );
	}

	void MeshTriangle::setPositions( const std::vector<Vec4f> & p_positions )
	{
		_updateBuffer( _vboPositions, p_positions );
	}

	void MeshTriangle::setNormals( const std::vector<Vec4f> & p_normals ) { _updateBuffer( _vboNormals, p_normals ); }

	void MeshTriangle::setColors( const std::vector<Color::Rgba> & p_colors ) { _updateBuffer( _vboColors, p_colors ); }

	void MeshTriangle::setVisibilities( const std::vector<uint> & p_visibilities )
	{
		_updateBuffer( _vboVisibilities, p_visibilities );
	}

	void MeshTriangle::setSelections( const std::vector<uint> & p_selections )
	{
		_updateBuffer( _vboSelections, p_selections );
	}

	void MeshTriangle::setIds( const std::vector<Model::ID> & p_ids ) { _updateBuffer( _vboIds, p_ids ); }

	void MeshTriangle::setIndices( const std::vector<uint> & p_indices ) { _updateBuffer( _ibo, p_indices ); }

} // namespace VTX::Buffer
