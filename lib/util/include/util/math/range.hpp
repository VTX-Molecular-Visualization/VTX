#ifndef __VTX_UTIL_MATH_RANGE__
#define __VTX_UTIL_MATH_RANGE__

#include <cassert>
#include <type_traits>
#include <util/math.hpp>
#include <vector>

namespace VTX::Util::Math
{
	template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
	class Range
	{
	  private:
		inline static const T ZERO = T( 0 );
		inline static const T ONE  = T( 1 );

	  public:
		inline static Range<T> createFirstLast( const T p_first, const T p_last )
		{
			return Range<T>( p_first, p_last - p_first + ONE );
		}

	  public:
		Range() : Range( ZERO, ZERO ) {}
		explicit Range( T p_start, T p_count = ONE ) : _start( p_start ), _count( p_count ) {}
		Range( const Range<T> & p_source ) : Range( p_source._start, p_source._count ) {}

		friend bool operator==( const Range<T> & p_lhs, const Range<T> & p_rhs )
		{
			return p_lhs._start == p_rhs._start && p_lhs._count == p_rhs._count;
		}
		friend bool operator!=( const Range<T> & p_lhs, const Range<T> & p_rhs )
		{
			return p_lhs._start != p_rhs._start || p_lhs._count != p_rhs._count;
		}

		T getFirst() const { return _start; };
		T getLast() const { return _start + _count - ONE; };
		T getCount() const { return _count; };

		bool contains( const T p_value ) const { return ( p_value >= _start ) && ( p_value < ( _start + _count ) ); }
		bool contains( const std::vector<T> & p_values ) const
		{
			for ( const T & value : p_values )
			{
				if ( !contains( value ) )
					return false;
			}

			return true;
		}
		bool contains( const Range<T> p_range ) const
		{
			return ( p_range._start >= _start ) && ( p_range.getLast() <= getLast() );
		}
		bool contains( const std::vector<Range<T>> p_ranges ) const
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( !contains( range ) )
					return false;
			}

			return true;
		}

		void add( const T p_count = ONE ) { _count += p_count; };
		void remove( const T p_count = ONE ) { _count -= p_count; };

		void setFirst( const T p_start )
		{
			assert( p_start < _start + _count );

			_count += _start - p_start;
			_start = p_start;
		};
		void setLast( const T p_last )
		{
			assert( p_last >= _start );
			_count = p_last - _start + 1;
		};

		void merge( const Range<T> & p_range )
		{
			// If invalid merge => create invalid range
			if ( ( p_range.getFirst() > ( getLast() + ONE ) ) || ( ( p_range.getLast() + ONE ) < getFirst() ) )
			{
				_count = ZERO;
				return;
			}

			const T newStart = p_range._start < _start ? p_range._start : _start;
			const T newLast	 = p_range.getLast() > getLast() ? p_range.getLast() : getLast();

			_start = newStart;
			_count = newLast - newStart + 1;
		};
		Range<T> intersect( const Range<T> & p_other )
		{
			if ( _start >= p_other._start + p_other._count || p_other._start >= _start + _count )
			{
				return Range<T>();
			}
			else
			{
				T start = Math::max( _start, p_other._start );
				T last	= Math::min( getLast(), p_other.getLast() );

				return createFirstLast( start, last );
			}
		};
		bool intersectWith( const Range<T> & p_other ) const
		{
			return !( _start >= p_other._start + p_other._count || p_other._start >= _start + _count );
		}

		bool isValid() const { return _count > ZERO; };

	  private:
		T _start;
		T _count;
	};
} // namespace VTX::Util::Math

#endif
