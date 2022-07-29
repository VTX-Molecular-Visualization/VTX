#ifndef __VTX_OBJECT_3D_HELPER_GRID__
#define __VTX_OBJECT_3D_HELPER_GRID__

#include "base_helper.hpp"
#include "renderer/gl/program_manager.hpp"

namespace VTX::Object3D::Helper
{
	class Grid : public BaseHelper
	{
	  public:
		Grid()	= default;
		~Grid() = default;

		Vec3f worldOrigin;
		Vec3f cellSize;
		Vec3i size;

		Vec3i gridPosition( const Vec3f & p_position ) const
		{
			VTX::Vec3i gridPos;
			gridPos.x = static_cast<int>( std::floor( ( p_position.x - worldOrigin.x ) / cellSize.x ) );
			gridPos.y = static_cast<int>( std::floor( ( p_position.y - worldOrigin.y ) / cellSize.y ) );
			gridPos.z = static_cast<int>( std::floor( ( p_position.z - worldOrigin.z ) / cellSize.z ) );
			return gridPos;
		};

		uint gridHash( Vec3i p_gridPosition ) const
		{
			p_gridPosition.x = p_gridPosition.x & ( size.x - 1 );
			p_gridPosition.y = p_gridPosition.y & ( size.y - 1 );
			p_gridPosition.z = p_gridPosition.z & ( size.z - 1 );
			return ( ( p_gridPosition.z * size.y ) * size.x ) + ( p_gridPosition.y * size.x ) + p_gridPosition.x;
		};

		void generate() override;
		void refresh();
		void render( const Object3D::Camera & ) const override;

		struct Voxel
		{
			Vec3f min;
			Vec3f max;
		};

	  private:
		enum ATTRIBUTE_LOCATION
		{
			VOXEL_MIN = 0,
			VOXEL_MAX = 1
		};

		Renderer::GL::BufferData _vbo = Renderer::GL::BufferData();
		// Renderer::GL::BufferData _ibo = Renderer::GL::BufferData();
		uint _size = 0;

		Renderer::GL::Program * _program = nullptr;
	};
} // namespace VTX::Object3D::Helper

#endif
