#include "box.hpp"

namespace VTX::Buffer
{
	void Box::generate()
	{
		_vbo.create();
		_ibo.create();

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		_vao.enableAttribute( ATTRIBUTE_LOCATION::AABB_CORNER );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::AABB_CORNER, _vbo, sizeof( Vec3f ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::AABB_CORNER, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::AABB_CORNER, ATTRIBUTE_LOCATION::AABB_CORNER );
	}

	void Box::setCorners( const std::vector<Vec3f> & p_positions )
	{
		_vbo.set<Vec3f>( p_positions, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

	void Box::setIndices( const std::vector<uint> & p_indices )
	{
		_ibo.set<uint>( p_indices, Renderer::GL::BufferData::Usage::STATIC_DRAW );
	}

} // namespace VTX::Buffer
