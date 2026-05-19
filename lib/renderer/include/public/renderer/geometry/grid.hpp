#ifndef __VTX_RENDERER_GEOMETRY_GRID__
#define __VTX_RENDERER_GEOMETRY_GRID__

#include "renderer/descriptors.hpp"
#include <limits>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Geometry
{
	class Grid : public Desc::Geometry
	{
	  public:
		inline static const Desc::Key VERTEX_LAYOUT_VOXELS = "Voxels";
		inline static const Desc::Key GEOMETRY_GRID		   = "Grid";
		inline static const Desc::Key INDIRECT_GRID		   = "Indirect.Grid";
		inline static constexpr Desc::Binding BINDING_INDIRECT_GRID = 23;

		Grid()
		{
			vertexLayout   = VERTEX_LAYOUT_VOXELS;
			indirectBuffer = INDIRECT_GRID;
		}

		void clear() { _voxelCount = 0; }

		void setVoxelCount( const size_t p_count )
		{
			if ( p_count > std::numeric_limits<uint32_t>::max() )
			{
				throw GraphicException( "Total grid voxel count exceeds maximum supported value." );
			}

			_voxelCount = static_cast<uint32_t>( p_count );
		}

		uint32_t voxelCount() const { return _voxelCount; }

		[[nodiscard]] std::vector<Desc::DrawIndirectRecord> toDrawIndirectCommands()
		{
			if ( _voxelCount == 0 )
			{
				return {};
			}

			return { Desc::DrawIndirectRecord { Desc::DrawIndirectCommand { _voxelCount, 1, 0, 0 }, 0 } };
		}

	  private:
		uint32_t _voxelCount = 0;
	};
} // namespace VTX::Renderer::Geometry

#endif
