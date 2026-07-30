#ifndef __VTX_UTIL_MATH_GRID__
#define __VTX_UTIL_MATH_GRID__

#include "util/types.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VTX::Util::Math
{
	/**
	 * @brief Describes a dense 3D grid in space, with a given origin and cell size.
	 */
	class DenseGrid
	{
	  public:
		using CellPosition = Vec3u;

		/**
		 * @brief Constructor.
		 */
		DenseGrid(
			const Vec3f & p_worldOrigin = Vec3f( 0.f ),
			const Vec3f & p_cellSize	= Vec3f( 10.f ),
			const Vec3u & p_size		= Vec3u( 0u )
		) : _worldOrigin( p_worldOrigin ), _cellSize( p_cellSize ), _size( p_size )
		{
		}

		/**
		 * @brief Get the center of a cell in world position.
		 */
		Vec3f worldPosition( const CellPosition & p_gridPosition ) const
		{
			Vec3f position = _worldOrigin;
			position += ( Vec3f( p_gridPosition ) + 0.5f ) * _cellSize;

			return position;
		}

		/**
		 * @brief Get the grid position of a world position.
		 */
		CellPosition gridPosition( const Vec3f & p_worldPosition ) const
		{
			CellPosition gridPos;

			gridPos.x = static_cast<uint>( std::floor( ( p_worldPosition.x - _worldOrigin.x ) / _cellSize.x ) );
			gridPos.y = static_cast<uint>( std::floor( ( p_worldPosition.y - _worldOrigin.y ) / _cellSize.y ) );
			gridPos.z = static_cast<uint>( std::floor( ( p_worldPosition.z - _worldOrigin.z ) / _cellSize.z ) );

			return gridPos;
		}

		/**
		 * @brief Get the grid position of a dense index in the grid.
		 */
		CellPosition gridPosition( const size_t p_denseIndex ) const
		{
			const size_t z = p_denseIndex / ( _size.x * _size.y );
			const size_t r = p_denseIndex % ( _size.x * _size.y );
			const size_t y = r / _size.x;
			const size_t x = r % _size.x;

			return CellPosition( x, y, z );
		}

		/**
		 * @brief Get the dense index of a cell position in the grid.
		 */
		size_t denseIndex( const CellPosition & p_gridPosition ) const
		{
			return p_gridPosition.z * _size.x * _size.y + p_gridPosition.y * _size.x + p_gridPosition.x;
		}

		size_t denseIndex( const uint p_x, const uint p_y, const uint p_z ) const
		{
			return denseIndex( CellPosition( p_x, p_y, p_z ) );
		}

		/**
		 * @brief Get the number of cells in the grid (fixed).
		 */
		size_t getCellCount() const { return _size.x * _size.y * _size.z; }

		/**
		 * @brief Grid size.
		 */
		const Vec3u & getSize() const { return _size; }

	  private:
		/**
		 * @brief Origin.
		 */
		Vec3f _worldOrigin;

		/**
		 * @brief Size of a cell.
		 */
		Vec3f _cellSize;

		/**
		 * @brief Size of the grid.
		 */
		Vec3u _size;
	};

	/**
	 * @brief Describes a sparse 3D grid in space, with a given origin and cell size.
	 * @tparam T The type of data stored in the grid.
	 */
	template<typename T>
	class Grid
	{
	  public:
		using CellPosition = Vec3i;
		using Cell		   = std::vector<T>;

		/**
		 * @brief Constructors.
		 */
		Grid( const Vec3f & p_worldOrigin = Vec3f( 0.f ), const Vec3f & p_cellSize = Vec3f( 8.f ) ) :
			_worldOrigin( p_worldOrigin ), _cellSize( p_cellSize )
		{
		}

		Grid( const Vec3f & p_worldOrigin ) : _worldOrigin( p_worldOrigin ) {}

		/**
		 * @brief Get the center of a cell in world position.
		 */
		Vec3f worldPosition( const CellPosition & p_gridPosition ) const
		{
			Vec3f position = _worldOrigin;
			position += ( Vec3f( p_gridPosition ) + 0.5f ) * _cellSize;

			return position;
		}

		/**
		 * @brief Get the grid position of a world position.
		 */
		CellPosition gridPosition( const Vec3f & p_worldPosition ) const
		{
			CellPosition gridPos;

			gridPos.x = static_cast<int>( std::floor( ( p_worldPosition.x - _worldOrigin.x ) / _cellSize.x ) );
			gridPos.y = static_cast<int>( std::floor( ( p_worldPosition.y - _worldOrigin.y ) / _cellSize.y ) );
			gridPos.z = static_cast<int>( std::floor( ( p_worldPosition.z - _worldOrigin.z ) / _cellSize.z ) );

			return gridPos;
		}

		/**
		 * @brief Get the number of occupied cells.
		 */
		inline size_t getCellCount() const { return _data.size(); }

		/**
		 * @brief Iterators.
		 */
		auto begin() { return _data.begin(); }

		auto end() { return _data.end(); }

		auto begin() const { return _data.begin(); }

		auto end() const { return _data.end(); }

		/**
		 * @brief Visitor function to iterate over all neighbour cells.
		 */
		template<typename F>
		void forEachNeighbourCell( const CellPosition & p_gridPosition, F && p_func, const int p_neighbourDist = 1 )
			const
		{
			for ( int z = -p_neighbourDist; z <= p_neighbourDist; ++z )
			{
				for ( int y = -p_neighbourDist; y <= p_neighbourDist; ++y )
				{
					for ( int x = -p_neighbourDist; x <= p_neighbourDist; ++x )
					{
						const CellPosition neighbourPosition = p_gridPosition + CellPosition( x, y, z );
						const auto		   it				 = _data.find( neighbourPosition );

						if ( it != _data.end() )
						{
							p_func( it->first, it->second );
						}
					}
				}
			}
		}

		template<typename F>
		void forEachNeighbourCellAt( const Vec3f & p_worldPosition, F && p_func ) const
		{
			forEachNeighbourCell( gridPosition( p_worldPosition ), std::forward<F>( p_func ) );
		}

		/**
		 * @brief Get the grid size derived from occupied cells bounds.
		 */
		Vec3u getSize() const
		{
			if ( _data.empty() )
			{
				return Vec3u( 0 );
			}

			const CellPosition size = _maxCell - _minCell + CellPosition( 1 );

			return Vec3u( size );
		}

		/**
		 * @brief Get the bounds of occupied cells.
		 */
		const CellPosition & getMinCell() const { return _minCell; }

		const CellPosition & getMaxCell() const { return _maxCell; }

		/**
		 * @brief Get the dense index of a cell position in the grid.
		 * Useful to store in 1D array.
		 */
		size_t denseIndex( const CellPosition & p_gridPosition ) const
		{
			const CellPosition localPosition = p_gridPosition - _minCell;
			const Vec3u		   size			 = getSize();

			return localPosition.z * size.x * size.y + localPosition.y * size.x + localPosition.x;
		}

		/**
		 * @brief Add a value to the grid at the given world position.
		 */
		void add( const T & p_value, const Vec3f & p_worldPosition )
		{
			add( p_value, gridPosition( p_worldPosition ) );
		}

		void add( const T & p_value, const CellPosition & p_gridPosition )
		{
			auto [ cell, inserted ] = _data.try_emplace( p_gridPosition );
			if ( inserted )
			{
				_extend( p_gridPosition );
			}

			cell->second.emplace_back( p_value );
		}

	  private:
		/**
		 * @brief Origin.
		 */
		Vec3f _worldOrigin;

		/**
		 * @brief Size of a cell.
		 */
		Vec3f _cellSize;

		/**
		 * @brief Bounds of occupied cells.
		 */
		CellPosition _minCell = CellPosition( 0 );
		CellPosition _maxCell = CellPosition( 0 );

		/**
		 * @brief Hash function for CellPosition to be used in unordered_map.
		 */
		struct CellPositionHasher
		{
			size_t operator()( const CellPosition & p_position ) const noexcept
			{
				return std::hash<int> {}( p_position.x ) ^ ( std::hash<int> {}( p_position.y ) << 1 )
					   ^ ( std::hash<int> {}( p_position.z ) << 2 );
			}
		};

		/**
		 * @brief Stored data in the grid, indexed by cell position.
		 */
		std::unordered_map<CellPosition, Cell, CellPositionHasher> _data;

		void _extend( const CellPosition & p_gridPosition )
		{
			if ( _data.size() == 1 )
			{
				_minCell = p_gridPosition;
				_maxCell = p_gridPosition;
				return;
			}

			_minCell.x = std::min( _minCell.x, p_gridPosition.x );
			_minCell.y = std::min( _minCell.y, p_gridPosition.y );
			_minCell.z = std::min( _minCell.z, p_gridPosition.z );

			_maxCell.x = std::max( _maxCell.x, p_gridPosition.x );
			_maxCell.y = std::max( _maxCell.y, p_gridPosition.y );
			_maxCell.z = std::max( _maxCell.z, p_gridPosition.z );
		}
	};
} // namespace VTX::Util::Math

#endif
