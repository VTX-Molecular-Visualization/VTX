#ifndef __VTX_APP_SYSTEM_GRIDATOMLIST__
#define __VTX_APP_SYSTEM_GRIDATOMLIST__

#include <unordered_map>
#include <vector>

namespace VTX::App::System
{
	struct Voxel
	{
		std::vector<size_t> atoms;
	};

	struct GridCoord
	{
		int x, y, z;

		bool operator==( const GridCoord & other ) const { return x == other.x && y == other.y && z == other.z; }
	};

	struct GridCoordHash
	{
		size_t operator()( const GridCoord & c ) const
		{
			return std::hash<int>()( c.x ) ^ ( std::hash<int>()( c.y ) << 1 ) ^ ( std::hash<int>()( c.z ) << 2 );
		}
	};

	struct GridAtomList
	{
		std::unordered_map<GridCoord, Voxel, GridCoordHash> grid;
	};
} // namespace VTX::App::System

#endif