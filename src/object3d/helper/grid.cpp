#include "grid.hpp"
#include "object3d/camera.hpp"

namespace VTX::Object3D::Helper
{
	void Grid::refresh()
	{
		std::vector<Voxel> voxels;
		voxels.reserve( size.x * size.y * size.z );
		for ( std::size_t x = 0; x < size.x; x++ )
		{
			for ( std::size_t y = 0; y < size.y; y++ )
			{
				for ( std::size_t z = 0; z < size.z; z++ )
				{
					const VTX::Vec3f min = ( Vec3f( x, y, z ) * cellSize ) + worldOrigin;
					voxels.emplace_back( Voxel { min, min + cellSize } );
				}
			}
		}

		_vbo.set<Voxel>( voxels, Renderer::GL::BufferData::Usage::STATIC_DRAW );
		_size = uint( voxels.size() );
	}

	void Grid::generate()
	{
		_program = VTX_PROGRAM_MANAGER().createProgram( "Voxel",
														{ IO::FilePath( "voxel/voxel.vert" ),
														  IO::FilePath( "voxel/voxel.geom" ),
														  IO::FilePath( "voxel/voxel.frag" ) } );
		assert( _program != nullptr );

		_vbo.create();
		//_ibo.create();

		_vao.create();

		//_vao.bindElementBuffer( _ibo );

		_vao.enableAttribute( ATTRIBUTE_LOCATION::VOXEL_MIN );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VOXEL_MIN, _vbo, sizeof( Voxel ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VOXEL_MIN, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VOXEL_MIN, ATTRIBUTE_LOCATION::VOXEL_MIN );

		_vao.enableAttribute( ATTRIBUTE_LOCATION::VOXEL_MAX );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::VOXEL_MAX, _vbo, sizeof( Voxel ), offsetof( Voxel, max.x ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::VOXEL_MAX, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::VOXEL_MAX, ATTRIBUTE_LOCATION::VOXEL_MAX );

		refresh();
	}

	void Grid::render( const Object3D::Camera & p_camera ) const
	{
		_program->use();

		const Mat4f MVMatrix = p_camera.getViewMatrix();
		_program->setMat4f( "u_MVMatrix", MVMatrix );
		_program->setMat4f( "u_projMatrix", p_camera.getProjectionMatrix() );
		_program->setMat4f( "u_normalMatrix", Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) );

		_vao.bind();
		_vao.drawArray( Renderer::GL::VertexArray::DrawMode::POINTS, 0, GLsizei( _size ) );
		_vao.unbind();
	}

} // namespace VTX::Object3D::Helper
