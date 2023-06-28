#ifndef __VTX_APP_OLD_COMPONENT_OBJECT3D_HELPER_GRID__
#define __VTX_APP_OLD_COMPONENT_OBJECT3D_HELPER_GRID__

#include "app/old/render/renderer/gl/program_manager.hpp"
#include "base_helper.hpp"

namespace VTX::App::Old::Component::Object3D::Helper
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
		void render( const App::Old::Component::Render::Camera & ) const override;

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

		App::Old::Render::Renderer::GL::Buffer _vbo	= App::Old::Render::Renderer::GL::Buffer();
		size_t							  _size = 0;

		App::Old::Render::Renderer::GL::Program * _program = nullptr;
	};
} // namespace VTX::App::Old::Component::Object3D::Helper

#endif
