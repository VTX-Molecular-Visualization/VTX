#include "mesh_triangle.hpp"

namespace VTX::Buffer
{
	void MeshTriangle::_generate()
	{
		_vboPositions.create();
		_vboNormals.create();
		_vboColors.create();
		_vboVisibilities.create();
		_ibo.create();

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		// Position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_POSITION, _vboPositions, sizeof( Vec3f ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_POSITION, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_POSITION, ATTRIBUTE_LOCATION::VERTEX_POSITION );

		// Normal.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_NORMAL );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_NORMAL, _vboNormals, sizeof( Vec3f ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_NORMAL, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_NORMAL, ATTRIBUTE_LOCATION::VERTEX_NORMAL );

		// Color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_COLOR, _vboColors, sizeof( Color::Rgb ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VERTEX_COLOR, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_COLOR, ATTRIBUTE_LOCATION::VERTEX_COLOR );

		// Visbility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, _vboVisibilities, sizeof( ushort ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_SHORT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VERTEX_VISIBILITY, ATTRIBUTE_LOCATION::VERTEX_VISIBILITY );
	}

	void MeshTriangle::bind() { _vao.bind(); }
	void MeshTriangle::unbind() { _vao.unbind(); }

	void MeshTriangle::setPositions( const std::vector<Vec3f> & p_positions )
	{
		_vboPositions.set<Vec3f>( p_positions, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void MeshTriangle::setNormals( const std::vector<Vec3f> & p_normals )
	{
		_vboNormals.set<Vec3f>( p_normals, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void MeshTriangle::setColors( const std::vector<Color::Rgb> & p_colors )
	{
		_vboColors.set<Color::Rgb>( p_colors, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void MeshTriangle::setVisibilities( const std::vector<ushort> & p_visibilities )
	{
		_vboVisibilities.set<ushort>( p_visibilities, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void MeshTriangle::setIndices( const std::vector<uint> & p_indices )
	{
		_ibo.set<uint>( p_indices, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

} // namespace VTX::Buffer
