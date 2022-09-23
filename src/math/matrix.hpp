#ifndef __VTX_MATH_MATRIX__
#define __VTX_MATH_MATRIX__

#include <valarray>

namespace VTX::Math
{
	template<typename T>
	class Matrix
	{
	  public:
		static Matrix<T> getIdentity( const size_t p_size )
		{
			Matrix<T> res = Matrix( p_size, p_size );

			for ( size_t i = 0; i < p_size; i++ )
				res.set( i, i, 1 );

			return res;
		}

	  public:
		Matrix() : Matrix( 0, 0 ) {}

		Matrix( const size_t p_row, const size_t p_col, const T defaultValue = T() )
		{
			_array		 = std::valarray<T>( defaultValue, p_row * p_col );
			_rowCount	 = p_row;
			_columnCount = p_col;
		}

		Matrix( const Matrix<T> & p_from )
		{
			_array		 = std::valarray<T>( p_from.getRowCount() * p_from.getColumnCount() );
			_rowCount	 = p_from.getRowCount();
			_columnCount = p_from.getColumnCount();

			for ( size_t iRow = 0; iRow < _rowCount; iRow++ )
			{
				for ( size_t iCol = 0; iCol < _columnCount; iCol++ )
				{
					const size_t valArrayIndex = _getArrayIndex( iRow, iCol );
					_array[ valArrayIndex ]	   = p_from._array[ valArrayIndex ];
				}
			}
		}

		const T & get( const size_t p_row, const size_t p_col ) const
		{
			return _array[ _getArrayIndex( p_row, p_col ) ];
		};
		void set( const size_t p_row, const size_t p_col, const T & p_value )
		{
			_array[ _getArrayIndex( p_row, p_col ) ] = p_value;
		};
		void add( const size_t p_row, const size_t p_col, const T & p_value )
		{
			_array[ _getArrayIndex( p_row, p_col ) ] += p_value;
		};

		size_t getRowCount() const { return _rowCount; };
		size_t getColumnCount() const { return _columnCount; };

	  private:
		size_t _getArrayIndex( const size_t p_row, const size_t p_col ) const { return _columnCount * p_row + p_col; }

		std::valarray<T> _array;
		size_t			 _rowCount;
		size_t			 _columnCount;
	};
} // namespace VTX::Math

#endif
