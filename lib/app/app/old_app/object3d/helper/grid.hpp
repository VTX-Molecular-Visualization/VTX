#ifndef __VTX_OBJECT_3D_HELPER_GRID__
#define __VTX_OBJECT_3D_HELPER_GRID__

#include "base_helper.hpp"
#include "old_app/renderer/gl/program_manager.hpp"

namespace VTX::Object3D::Helper
{
	class Grid : public BaseHelper
	{
	  public:
		Grid() = default;
		Grid( const Vec3f & p_worldOrigin, const Vec3f & p_cellSize, const Vec3i & p_size ) :
			worldOrigin( p_worldOrigin ), cellSize( p_cellSize ), size( p_size )
		{
		}
		~Grid() = default;

		Vec3f worldOrigin;
		Vec3f cellSize;
		Vec3i size;

		Vec3f worldPosition( const Vec3i & p_gridPosition ) const;
		Vec3f worldPosition( const uint p_hash ) const;
		Vec3i gridPosition( const Vec3f & p_worldPosition ) const;
		Vec3i gridPosition( const uint p_hash ) const;
		uint  gridHash( const Vec3f & p_worldPosition ) const;
		uint  gridHash( const Vec3i & p_gridPosition ) const;
		uint  gridHash( const uint p_x, const uint p_y, const uint p_z ) const;

		inline const size_t getCellCount() const { return size.x * size.y * size.z; }

		void refresh() override;
		void render( const Object3D::Camera & ) const override;

		struct Voxel
		{
			Vec3f min;
			Vec3f max;
		};

	  protected:
		void _generate() override;

	  private:
		enum ATTRIBUTE_LOCATION
		{
			VOXEL_MIN = 0,
			VOXEL_MAX = 1
		};

		Renderer::GL::Buffer _vbo  = Renderer::GL::Buffer();
		size_t				 _size = 0;

		Renderer::GL::Program * _program = nullptr;
	};
} // namespace VTX::Object3D::Helper

#endif
