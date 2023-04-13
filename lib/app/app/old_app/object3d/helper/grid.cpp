#include "grid.hpp"
#include "old_app/object3d/camera.hpp"

namespace VTX::Object3D::Helper
{
	Vec3f Grid::worldPosition( const Vec3i & p_gridPosition ) const
	{
		VTX::Vec3f position = worldOrigin;
		position += ( Vec3f( p_gridPosition ) + 0.5f ) * cellSize;
		return position;
	}

	Vec3f Grid::worldPosition( const uint p_hash ) const { return worldPosition( gridPosition( p_hash ) ); }

	Vec3i Grid::gridPosition( const Vec3f & p_worldPosition ) const
	{
		VTX::Vec3i gridPos;
		gridPos.x = std::floor( ( p_worldPosition.x - worldOrigin.x ) / cellSize.x );
		gridPos.y = std::floor( ( p_worldPosition.y - worldOrigin.y ) / cellSize.y );
		gridPos.z = std::floor( ( p_worldPosition.z - worldOrigin.z ) / cellSize.z );
		return gridPos;
	}

	Vec3i Grid::gridPosition( const uint p_hash ) const
	{
		const uint z = p_hash / ( size.x * size.y );
		uint	   r = p_hash % ( size.x * size.y );
		const uint y = r / size.x;
		const uint x = r % size.x;

		return Vec3i( x, y, z );
	}

	uint Grid::gridHash( const Vec3f & p_worldPosition ) const { return gridHash( gridPosition( p_worldPosition ) ); }

	uint Grid::gridHash( const Vec3i & p_gridPosition ) const
	{
		return ( p_gridPosition.z * size.x * size.y ) + ( p_gridPosition.y * size.x ) + p_gridPosition.x;
	}

	uint Grid::gridHash( const uint p_x, const uint p_y, const uint p_z ) const
	{
		return ( p_z * size.x * size.y ) + ( p_y * size.x ) + p_x;
	}

	void Grid::refresh()
	{
		std::vector<Voxel> voxels;
		voxels.resize( size.x * size.y * size.z );
		for ( int x = 0; x < size.x; x++ )
		{
			for ( int y = 0; y < size.y; y++ )
			{
				for ( int z = 0; z < size.z; z++ )
				{
					const VTX::Vec3f min		  = ( Vec3f( x, y, z ) * cellSize ) + worldOrigin;
					voxels[ gridHash( x, y, z ) ] = Voxel { min, min + cellSize };
				}
			}
		}

		_vbo.set<Voxel>( voxels, Renderer::GL::Buffer::Usage::STATIC_DRAW );
		_size = uint( voxels.size() );
	}

	void Grid::_generate()
	{
		_program = VTX_PROGRAM_MANAGER().createProgram( "Voxel",
														{ FilePath( "voxel/voxel.vert" ),
														  FilePath( "voxel/voxel.geom" ),
														  FilePath( "voxel/voxel.frag" ) } );
		assert( _program != nullptr );

		_vbo.create();
		_vao.create();

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
