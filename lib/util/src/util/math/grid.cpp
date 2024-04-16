#include "util/math/grid.hpp"

namespace VTX::Util::Math
{
	Vec3f Grid::worldPosition( const Vec3i & p_gridPosition ) const
	{
		Vec3f position = worldOrigin;
		position += ( Vec3f( p_gridPosition ) + 0.5f ) * cellSize;
		return position;
	}

	Vec3f Grid::worldPosition( const uint p_hash ) const { return worldPosition( gridPosition( p_hash ) ); }

	Vec3i Grid::gridPosition( const Vec3f & p_worldPosition ) const
	{
		Vec3i gridPos;
		gridPos.x = int( std::floor( ( p_worldPosition.x - worldOrigin.x ) / cellSize.x ) );
		gridPos.y = int( std::floor( ( p_worldPosition.y - worldOrigin.y ) / cellSize.y ) );
		gridPos.z = int( std::floor( ( p_worldPosition.z - worldOrigin.z ) / cellSize.z ) );
		return gridPos;
	}

	Vec3i Grid::gridPosition( const uint p_hash ) const
	{
		const uint z = p_hash / ( size.x * size.y );
		const uint r = p_hash % ( size.x * size.y );
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

	std::pair<std::vector<Vec3f>, std::vector<Vec3f>> Grid::toVoxels() const
	{
		std::vector<Vec3f> mins( size.x * size.y * size.z );
		std::vector<Vec3f> maxs( size.x * size.y * size.z );

		for ( int x = 0; x < size.x; x++ )
		{
			for ( int y = 0; y < size.y; y++ )
			{
				for ( int z = 0; z < size.z; z++ )
				{
					const VTX::Vec3f min		= ( Vec3f( x, y, z ) * cellSize ) + worldOrigin;
					mins[ gridHash( x, y, z ) ] = min;
					maxs[ gridHash( x, y, z ) ] = min + cellSize;
				}
			}
		}

		return std::make_pair( mins, maxs );
	}

} // namespace VTX::Util::Math
