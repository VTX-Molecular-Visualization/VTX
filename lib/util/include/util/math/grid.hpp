#ifndef __VTX_UTIL_MATH_GRID__
#define __VTX_UTIL_MATH_GRID__

#include "util/types.hpp"
#include <vector>

namespace VTX::Util::Math
{
	class Grid
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

		std::pair<std::vector<Vec3f>, std::vector<Vec3f>> toVoxels() const;

		inline const size_t getCellCount() const { return size.x * size.y * size.z; }
	};
} // namespace VTX::Util::Math

#endif
