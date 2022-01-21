#ifndef __VTX_MATH_GRID__
#define __VTX_MATH_GRID__

#include "define.hpp"

namespace VTX
{
	namespace Math
	{
		struct Grid
		{
			Grid()	= default;
			~Grid() = default;

			Vec3i gridPosition( const Vec3f & p_position ) const
			{
				return Vec3i( glm::floor( ( p_position - worldOrigin ) / cellSize ) );
			}

			uint gridHash( const Vec3i & p_gridPosition ) const
			{
				Vec3i pos = p_gridPosition;

				pos.x = pos.x & ( size.x - 1 );
				pos.y = pos.y & ( size.y - 1 );
				pos.z = pos.z & ( size.z - 1 );
				return ( ( pos.z * size.y ) * size.x ) + ( pos.y * size.x ) + pos.x;
			}

			Vec3f worldOrigin;
			Vec3f cellSize;
			Vec3i size;
		};
	} // namespace Math
} // namespace VTX

#endif
