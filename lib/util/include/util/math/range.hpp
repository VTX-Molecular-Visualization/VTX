#ifndef __VTX_UTIL_MATH_RANGE__
#define __VTX_UTIL_MATH_RANGE__

#include "util/constants.hpp"
#include <cassert>
#include <concepts>
#include <optional>

namespace VTX::Util::Math
{
	/**
	 * @brief Simple integral range [first, last)
	 */
	template<std::integral T>
	struct Range
	{
		using Count = std::make_unsigned_t<T>;

		/**
		 * @brief Enum to specify range edge.
		 */
		enum class E_EDGE
		{
			FIRST,
			LAST
		};

		/**
		 * @brief First value of the range (inclusive).
		 */
		T first = 0;

		/**
		 * @brief Last value of the range (exclusive).
		 */
		T last = 0;

		/**
		 * @brief Default constructor creates an invalid range (first == last == 0).
		 */
		constexpr Range() noexcept = default;

		/**
		 * @brief Constructs a range containing a single value.
		 */
		constexpr Range( const T p_first ) noexcept : first( p_first ), last( static_cast<T>( p_first + 1 ) )
		{
			assert( p_first < TypeMax<T> );
		}

		/**
		 * @brief Constructs a range from first (inclusive) to last (exclusive).
		 */
		constexpr Range( const T p_first, const T p_last ) noexcept :
			first( std::min( p_first, p_last ) ), last( std::max( p_first, p_last ) )
		{
		}

		/**
		 * @brief Constructs a range from first (inclusive) and count.
		 */
		[[nodiscard]] static constexpr Range fromFirstCount( const T p_first, const Count p_count ) noexcept
		{
			// Overflow.
			assert( p_first <= TypeMax<T> - static_cast<T>( p_count ) );
			return Range( p_first, static_cast<T>( p_first + static_cast<T>( p_count ) ) );
		}

		/**
		 * @brief Constructs a closed range [first, last] (both inclusive).
		 */
		[[nodiscard]] static constexpr Range closed( const T p_first, const T p_lastInclusive ) noexcept
		{
			assert( p_lastInclusive < TypeMax<T> );
			return Range( p_first, static_cast<T>( p_lastInclusive + 1 ) );
		}

		/**
		 * @brief Get first or last value.
		 */
		constexpr T getFirst() const noexcept { return first; }
		constexpr T getLast() const noexcept { return last; }

		/**
		 * @brief Get count of values in the range.
		 */
		constexpr Count getCount() const noexcept { return last - first; }

		/**
		 * @brief Checks the range validity (first <= last).
		 */
		constexpr bool isValid() const noexcept { return first <= last; }

		/**
		 * @brief Checks the range emptiness (first >= last).
		 */
		constexpr bool isEmpty() const noexcept { return first >= last; }

		/**
		 * @brief Checks if the range contains a value.
		 */
		constexpr bool contains( const T p_value ) const noexcept { return p_value >= first && p_value < last; }

		/**
		 * @brief Checks if the range contains another range.
		 */
		constexpr bool contains( const Range & p_other ) const noexcept
		{
			return p_other.first >= first && p_other.last <= last;
		}

		/**
		 * @brief Checks if the range contains all values in a vector.
		 */
		constexpr bool contains( const std::vector<T> & p_values ) const noexcept
		{
			for ( const T & value : p_values )
			{
				if ( not contains( value ) )
				{
					return false;
				}
			}

			return true;
		}

		/**
		 * @brief Checks if the range contains all values in an initializer list.
		 */
		constexpr bool contains( const std::initializer_list<T> & p_values ) const noexcept
		{
			for ( const T & value : p_values )
			{
				if ( not contains( value ) )
				{
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Checks if the range contains all ranges in a vector.
		 */
		constexpr bool contains( const std::vector<Range<T>> & p_ranges ) const noexcept
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( not contains( range ) )
				{
					return false;
				}
			}

			return true;
		}

		/**
		 * @brief Checks if the range intersects another range.
		 */
		constexpr bool intersects( const Range & p_other ) const noexcept
		{
			return not( last <= p_other.first || p_other.last <= first );
		}

		/**
		 * @brief Checks if the range touches another range (i.e. edges are contiguous).
		 */
		constexpr bool touches( const Range & p_other ) const noexcept
		{
			return last == p_other.first || p_other.last == first;
		}

		/**
		 * @brief Computes the intersection of this range with another range.
		 */
		[[nodiscard]] constexpr std::optional<Range> intersection( const Range & p_other ) const noexcept
		{
			const T f = std::max( first, p_other.first );
			const T l = std::min( last, p_other.last );
			if ( f >= l )
			{
				return std::nullopt;
			}
			return Range( f, l );
		}

		/**
		 * @brief Computes the union of this range with another range if they overlap or touch.
		 */
		[[nodiscard]] constexpr std::optional<Range> merge( const Range & p_other ) const noexcept
		{
			if ( intersects( p_other ) || touches( p_other ) )
			{
				return Range( std::min( first, p_other.first ), std::max( last, p_other.last ) );
			}
			return std::nullopt;
		}

		/**
		 * @brief Merges another range into this one if they overlap or touch.
		 */
		constexpr bool mergeInPlace( const Range & p_other ) noexcept
		{
			if ( intersects( p_other ) || touches( p_other ) )
			{
				first = std::min( first, p_other.first );
				last  = std::max( last, p_other.last );
				return true;
			}
			return false;
		}

		/**
		 * @brief Expands the range by adding a value.
		 */
		constexpr void add( const T p_value ) noexcept
		{
			if ( isEmpty() )
			{
				first = p_value;
				assert( p_value < TypeMax<T> );
				last = static_cast<T>( p_value + 1 );
				return;
			}
			if ( p_value < first )
			{
				first = p_value;
			}
			if ( p_value >= last )
			{
				assert( p_value < TypeMax<T> );
				last = static_cast<T>( p_value + 1 );
			}
		}

		/**
		 * @brief Expands the range by adding another range.
		 */
		constexpr void add( const Range & p_other ) noexcept
		{
			if ( isEmpty() )
			{
				*this = p_other;
				return;
			}
			first = std::min( first, p_other.first );
			last  = std::max( last, p_other.last );
		}

		/**
		 * @brief Shrinks the range by removing a value.
		 */
		constexpr void remove( const T p_value ) noexcept
		{
			if ( isEmpty() )
			{
				return;
			}
			if ( p_value == first )
			{
				first++;
			}
			else if ( p_value == last - 1 )
			{
				last--;
			}
			if ( first >= last )
			{
				*this = {};
			}
		}

		/**
		 * @brief Shrinks the range by removing another range.
		 */
		constexpr void remove( const Range & p_other ) noexcept
		{
			if ( not intersects( p_other ) )
			{
				return;
			}

			if ( p_other.first <= first && p_other.last >= last )
			{
				first = last = 0;
				return;
			}

			if ( p_other.first <= first && p_other.last < last )
			{
				first = p_other.last;
				return;
			}

			if ( p_other.first > first && p_other.last >= last )
			{
				last = p_other.first;
				return;
			}

			last = p_other.first;
		}

		/**
		 * @brief Expand the range by n from the chosen edge.
		 */
		constexpr void expand( const Count p_n, const E_EDGE p_where ) noexcept
		{
			if ( p_n == 0 )
			{
				return;
			}

			if ( p_where == E_EDGE::FIRST )
			{
				// Overflow: first - n.
				if constexpr ( std::is_unsigned_v<T> )
				{
					assert( static_cast<Count>( first ) >= p_n );
				}
				else
				{
					assert( first >= TypeLowest<T> + static_cast<T>( p_n ) );
				}
				first = static_cast<T>( first - static_cast<T>( p_n ) );
			}
			else // Edge::Last
			{
				// Overflow: last + n.
				assert( last <= TypeMax<T> - static_cast<T>( p_n ) );
				last = static_cast<T>( last + static_cast<T>( p_n ) );
			}
		}

		/**
		 * @brief Shrink the range by n from the chosen edge.
		 */
		constexpr void shrink( const Count p_n, const E_EDGE p_where ) noexcept
		{
			if ( p_n == 0 || isEmpty() )
			{
				return;
			}

			const Count c = static_cast<Count>( getCount() );
			const Count k = ( p_n >= c ) ? c : p_n; // Clamp.

			if ( p_where == E_EDGE::FIRST )
			{
				// First += k
				assert( first <= TypeMax<T> - static_cast<T>( k ) );
				first = static_cast<T>( first + static_cast<T>( k ) );
			}
			else // Edge::Last
			{
				// Last -= k
				if constexpr ( std::is_unsigned_v<T> )
				{
					assert( static_cast<Count>( last ) >= k );
				}
				else
				{
					assert( last >= TypeLowest<T> + static_cast<T>( k ) );
				}
				last = static_cast<T>( last - static_cast<T>( k ) );
			}
		}

		/**
		 * @brief Convenience expand/shrink functions.
		 */
		constexpr void expandFirst( const Count p_n ) noexcept { expand( p_n, E_EDGE::FIRST ); }
		constexpr void expandLast( const Count p_n ) noexcept { expand( p_n, E_EDGE::LAST ); }
		constexpr void shrinkFirst( const Count p_n ) noexcept { shrink( p_n, E_EDGE::FIRST ); }
		constexpr void shrinkLast( const Count p_n ) noexcept { shrink( p_n, E_EDGE::LAST ); }

		/**
		 * @brief Equality operator.
		 */
		friend constexpr bool operator==( const Range &, const Range & ) = default;

		/**
		 * @brief Three-way comparison operator.
		 */
		friend constexpr auto operator<=>( const Range & p_a, const Range & p_b ) noexcept
		{
			if ( p_a.first != p_b.first )
			{
				return p_a.first <=> p_b.first;
			}
			return p_a.last <=> p_b.last;
		}

		/**
		 * @brief Iterator over the range values.
		 */
		struct iterator
		{
			T cur {};

			constexpr T			 operator*() const noexcept { return cur; }
			constexpr iterator & operator++() noexcept
			{
				++cur;
				return *this;
			}
			constexpr bool operator!=( const iterator & other ) const noexcept { return cur != other.cur; }
		};

		constexpr iterator begin() const noexcept { return iterator { first }; }
		constexpr iterator end() const noexcept { return iterator { last }; }
	};

} // namespace VTX::Util::Math

#endif
