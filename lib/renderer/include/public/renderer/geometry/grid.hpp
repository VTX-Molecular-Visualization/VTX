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
		Grid()
		{
			vertexLayout   = "Voxels";
			indirectBuffer = "Indirect.Grid";
		}

		uint32_t count = 0;

		void clear()
		{
			_voxelCount = 0;
			count		= 0;
		}

		void setVoxelCount( const size_t p_count )
		{
			if ( p_count > std::numeric_limits<uint32_t>::max() )
			{
				throw GraphicException( "Total grid voxel count exceeds maximum supported value." );
			}

			_voxelCount = static_cast<uint32_t>( p_count );
			count		= _voxelCount == 0 ? 0 : 1;
		}

		uint32_t voxelCount() const { return _voxelCount; }

		[[nodiscard]] std::vector<Desc::DrawIndirectCommand> toDrawIndirectCommands()
		{
			count = _voxelCount == 0 ? 0 : 1;

			if ( _voxelCount == 0 )
			{
				return {};
			}

			return { Desc::DrawIndirectCommand { _voxelCount, 1, 0, 0 } };
		}

	  private:
		uint32_t _voxelCount = 0;
	};
} // namespace VTX::Renderer::Geometry

#endif
