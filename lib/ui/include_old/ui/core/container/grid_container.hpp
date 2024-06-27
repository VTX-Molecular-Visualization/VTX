#ifndef __VTX_UI_CORE_CONTAINER_GRID__
#define __VTX_UI_CORE_CONTAINER_GRID__

#include "ui/core/define.hpp"
#include <utility>
#include <vector>

namespace VTX::UI::Core::Container
{
	template<typename T>
	class GridContainer
	{
	  protected:
		using Column = std::vector<T *>;
		using Grid	 = std::vector<Column>;

	  public:
		GridContainer() { _addNewColumn(); }

		void appendItem( T * const p_item )
		{
			_gridContainer.rbegin()->emplace_back( p_item );
			_size++;
		}
		void appendItemInNewColumn( T * const p_item )
		{
			_addNewColumn();
			appendItem( p_item );
		}
		void clear()
		{
			_gridContainer = Grid();
			_currentColumn = 0;
			_currentLine   = 0;
			_size		   = 0;
		}

		size_t size() const { return _size; }
		int	   getColumnCount() const { return int( _gridContainer.size() ); }

		T * getItemNeighbour( const T * const p_from, const Direction & p_direction, const bool p_loop = true ) const
		{
			switch ( p_direction )
			{
			case Direction::Down: getDownItem( p_from, p_loop ); break;
			case Direction::Right: getRightItem( p_from, p_loop ); break;
			case Direction::Up: getUpItem( p_from, p_loop ); break;
			case Direction::Left: getLeftItem( p_from, p_loop ); break;
			}
		}

		T * getDownItem( const T * const p_from, const bool p_loop = true ) const
		{
			for ( int i = 0; i < _gridContainer.size(); i++ )
			{
				const Column & column = _gridContainer[ i ];

				for ( int j = 0; j < column.size(); j++ )
				{
					const T * const item = column[ j ];

					if ( item == p_from )
					{
						T * res;

						const int downItemColumnIndex = j + 1;
						if ( downItemColumnIndex >= column.size() )
						{
							res = p_loop ? column[ 0 ] : nullptr;
						}
						else
						{
							res = column[ downItemColumnIndex ];
						}

						return res;
					}
				}
			}

			return nullptr;
		}
		T * getUpItem( const T * const p_from, const bool p_loop = true ) const
		{
			for ( int i = 0; i < _gridContainer.size(); i++ )
			{
				const Column & column = _gridContainer[ i ];

				for ( int j = 0; j < column.size(); j++ )
				{
					const T * const item = column[ j ];

					if ( item == p_from )
					{
						T * res;

						const int upItemColumnIndex = j - 1;
						if ( upItemColumnIndex < 0 )
						{
							res = p_loop ? column[ column.size() - 1 ] : nullptr;
						}
						else
						{
							res = column[ upItemColumnIndex ];
						}

						return res;
					}
				}
			}

			return nullptr;
		}
		T * getLeftItem( const T * const p_from, const bool p_loop = true ) const
		{
			for ( int i = 0; i < _gridContainer.size(); i++ )
			{
				const Column & column = _gridContainer[ i ];

				for ( int j = 0; j < column.size(); j++ )
				{
					const T * const item = column[ j ];

					if ( item == p_from )
					{
						int indexLeftColumn = i - 1;
						if ( indexLeftColumn < 0 )
						{
							if ( p_loop )
							{
								indexLeftColumn = _gridContainer.size() - 1;
							}
							else
							{
								return nullptr;
							}
						}

						T * res;

						const int sizeCurrentColumn = column.size();
						const int sizeLeftColumn	= _gridContainer[ indexLeftColumn ].size();

						if ( j == sizeCurrentColumn - 1 )
						{
							res = _gridContainer[ indexLeftColumn ][ sizeLeftColumn - 1 ].second;
						}
						else
						{
							const float indexConversionFactor = sizeLeftColumn / float( sizeCurrentColumn );
							const int	indexInLeftColumn	  = int( j * indexConversionFactor );

							res = _gridContainer[ indexLeftColumn ][ indexInLeftColumn ].second;
						}

						return res;
					}
				}
			}

			return nullptr;
		}
		T * getRightItem( const T * const p_from, const bool p_loop = true ) const
		{
			for ( int i = 0; i < _gridContainer.size(); i++ )
			{
				const Column & column = _gridContainer[ i ];

				for ( int j = 0; j < column.size(); j++ )
				{
					const T * const item = column[ j ];

					if ( item == p_from )
					{
						int indexRightColumn = i + 1;
						if ( indexRightColumn >= _gridContainer.size() )
						{
							if ( p_loop )
							{
								indexRightColumn = 0;
							}
							else
							{
								return nullptr;
							}
						}

						T * res;

						const int sizeCurrentColumn = column.size();
						const int sizeRightColumn	= _gridContainer[ indexRightColumn ].size();

						if ( j == sizeCurrentColumn - 1 )
						{
							res = _gridContainer[ indexRightColumn ][ sizeRightColumn - 1 ].second;
						}
						else
						{
							const float indexConversionFactor = sizeRightColumn / float( sizeCurrentColumn );
							const int	indexInRightColumn	  = int( j * indexConversionFactor );

							res = _gridContainer[ indexRightColumn ][ indexInRightColumn ].second;
						}

						return res;
					}
				}
			}

			return nullptr;
		}

	  private:
		void _addNewColumn()
		{
			_gridContainer.emplace_back( Column() );
			_currentColumn++;
			_currentLine = 0;
		}

		Grid _gridContainer = Grid();

		size_t _currentColumn = 0;
		size_t _currentLine	  = 0;
		size_t _size		  = 0;
	};
} // namespace VTX::UI::Core::Container

#endif
