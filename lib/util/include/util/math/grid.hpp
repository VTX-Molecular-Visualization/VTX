#ifndef __VTX_UTIL_MATH_GRID__
#define __VTX_UTIL_MATH_GRID__

#include "util/types.hpp"
#include <cmath>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VTX::Util::Math
{
	/**
	 * @brief Describes a 3D grid in space, with a given origin, cell size and number of cells.
	 * @tparam T The type of data stored in the grid.
	 */
	template<typename T>
	class Grid
	{
	  public:
		using Hash = uint64_t;
		using Cell = std::vector<T>;

		/**
		 * @brief Constructors.
		 */
		Grid() = default;

		Grid( const Vec3f & p_worldOrigin, const Vec3f & p_cellSize, const Vec3u & p_size ) :
			_worldOrigin( p_worldOrigin ), _cellSize( p_cellSize ), _size( p_size )
		{
		}

		Grid( const Vec3f & p_worldOrigin, const float p_cellSize, const Vec3u & p_size ) :
			_worldOrigin( p_worldOrigin ), _cellSize( p_cellSize ), _size( p_size )
		{
		}

		/**
		 * @brief Get the center of a cell in world position.
		 */
		Vec3f worldPosition( const Vec3u & p_gridPosition ) const
		{
			Vec3f position = _worldOrigin;
			position += ( Vec3f( p_gridPosition ) + 0.5f ) * _cellSize;

			return position;
		}

		Vec3f worldPosition( const Hash p_hash ) const { return worldPosition( gridPosition( p_hash ) ); }

		/**
		 * @brief Get the grid position of a world position.
		 */
		Vec3u gridPosition( const Vec3f & p_worldPosition ) const
		{
			Vec3u gridPos;

			gridPos.x = static_cast<uint>( std::floor( ( p_worldPosition.x - _worldOrigin.x ) / _cellSize.x ) );
			gridPos.y = static_cast<uint>( std::floor( ( p_worldPosition.y - _worldOrigin.y ) / _cellSize.y ) );
			gridPos.z = static_cast<uint>( std::floor( ( p_worldPosition.z - _worldOrigin.z ) / _cellSize.z ) );

			return gridPos;
		}

		Vec3u gridPosition( const Hash p_hash ) const
		{
			const Hash z = p_hash / ( _size.x * _size.y );
			const Hash r = p_hash % ( _size.x * _size.y );
			const Hash y = r / _size.x;
			const Hash x = r % _size.x;

			return { x, y, z };
		}

		/**
		 * @brief Compute cell hash.
		 */
		Hash gridHash( const Vec3f & p_worldPosition ) const { return gridHash( gridPosition( p_worldPosition ) ); }

		Hash gridHash( const Vec3u & p_gridPosition ) const
		{
			return ( p_gridPosition.z * _size.x * _size.y ) + ( p_gridPosition.y * _size.x ) + p_gridPosition.x;
		}

		Hash gridHash( const uint p_x, const uint p_y, const uint p_z ) const
		{
			return ( p_z * _size.x * _size.y ) + ( p_y * _size.x ) + p_x;
		}

		/**
		 * @brief Convert the grid to a list of voxels (renderer ready for debugging).
		 */
		/*
		std::pair<std::vector<Vec3f>, std::vector<Vec3f>> toVoxels() const
		{
			std::vector<Vec3f> mins( _size.x * _size.y * _size.z );
			std::vector<Vec3f> maxs( _size.x * _size.y * _size.z );

			for ( int x = 0; x < _size.x; x++ )
			{
				for ( int y = 0; y < _size.y; y++ )
				{
					for ( int z = 0; z < _size.z; z++ )
					{
						const VTX::Vec3f min		= ( Vec3f( x, y, z ) * _cellSize ) + _worldOrigin;
						mins[ gridHash( x, y, z ) ] = min;
						maxs[ gridHash( x, y, z ) ] = min + _cellSize;
					}
				}
			}

			return std::make_pair( mins, maxs );
		}
		*/

		/**
		 * @brief Get the number of cells.
		 */
		inline const size_t getCellCount() const { return _size.x * _size.y * _size.z; }

		/**
		 * @brief Add a value to the grid at the given world position.
		 */
		void add( const T & p_value, const Vec3f & p_worldPosition ) { add( p_value, gridHash( p_worldPosition ) ); }

		void add( const T & p_value, const Vec3u & p_gridPosition ) { add( p_value, gridHash( p_gridPosition ) ); }

		void add( const T & p_value, const Hash p_hash )
		{
			auto & cell = _data[ p_hash ];
			cell.emplace_back( p_value );
		}

	  private:
		/**
		 * @brief Origin (minimum corner).
		 */
		Vec3f _worldOrigin;

		/**
		 * @brief Size of a cell.
		 */
		Vec3f _cellSize;

		/**
		 * @brief Size of the grid in number of cells.
		 */
		Vec3u _size;

		/**
		 * @brief Stored data in the grid, indexed by cell hash.
		 */
		std::unordered_map<Hash, Cell> _data;
	};
} // namespace VTX::Util::Math

#endif
