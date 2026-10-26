#ifndef VTX_UTIL_MATH_RANGE_LIST_HPP
#define VTX_UTIL_MATH_RANGE_LIST_HPP

#include "util/math/range.hpp"
#include <algorithm>
#include <concepts>
#include <span>
#include <type_traits>
#include <vector>

namespace VTX::Util::Math
{
	/**
	 * @brief List of integral ranges [first, last)
	 */
	template<std::integral T>
	class RangeList
	{
	  public:
		using RangeType = Range<T>;

		/**
		 * @brief Constructors.
		 */
		constexpr RangeList() = default;

		constexpr RangeList( const std::initializer_list<RangeType> p_ranges )
		{
			for ( const auto & r : p_ranges )
			{
				addRange( r );
			}
		}

		constexpr explicit RangeList( const std::initializer_list<T> p_values )
		{
			for ( const T v : p_values )
			{
				addValue( v );
			}
		}

		constexpr explicit RangeList( const std::span<const T> p_values )
		{
			for ( const T v : p_values )
			{
				addRange( v );
			}
		}

		constexpr explicit RangeList( const T p_values ) { addValue( p_values ); }

		constexpr explicit RangeList( const RangeType & p_range ) { addRange( p_range ); }

		/**
		 * @brief Add a single value.
		 */
		constexpr void addValue( const T p_v ) { addRange( RangeType { p_v, static_cast<T>( p_v + 1 ) } ); }

		/**
		 * @brief Add multiple values.
		 */
		constexpr void addValues( const std::initializer_list<T> p_values )
		{
			for ( const T v : p_values )
			{
				addValue( v );
			}
		}

		constexpr void addValues( const std::span<const T> p_values )
		{
			for ( const T v : p_values )
			{
				addValue( v );
			}
		}

		/**
		 * @brief Add a single range.
		 */
		constexpr void addRange( const RangeType & p_r )
		{
			if ( p_r.isEmpty() )
			{
				return;
			}

			RangeType r	 = p_r;
			auto	  it = _ranges.begin();
			while ( it != _ranges.end() && it->last < r.first )
			{
				++it;
			}

			while ( it != _ranges.end() && not( it->getFirst() > r.last || it->getLast() < r.first ) )
			{
				r.first = std::min( r.first, it->getFirst() );
				r.last	= std::max( r.last, it->getLast() );
				it		= _ranges.erase( it );
			}

			_ranges.insert( it, r );
		}

		/**
		 * @brief Remove a single value.
		 */
		constexpr void removeValue( const T p_v ) { removeRange( RangeType { p_v, static_cast<T>( p_v + 1 ) } ); }

		/**
		 * @brief Remove a single range.
		 */
		constexpr void removeRange( const RangeType & p_r )
		{
			if ( p_r.isEmpty() )
			{
				return;
			}

			auto it = _ranges.begin();
			while ( it != _ranges.end() )
			{
				if ( it->last <= p_r.first || it->first >= p_r.last )
				{
					++it;
					continue;
				}

				if ( p_r.first <= it->first && p_r.last >= it->last )
				{
					it = _ranges.erase( it );
					continue;
				}

				if ( p_r.first <= it->first && p_r.last < it->last )
				{
					it->first = p_r.last;
					++it;
					continue;
				}

				if ( p_r.first > it->first && p_r.last >= it->last )
				{
					it->last = p_r.first;
					++it;
					continue;
				}

				const RangeType right { p_r.last, it->last };
				it->last = p_r.first;
				it		 = _ranges.insert( std::next( it ), right );
				++it;
			}
		}

		/**
		 * @brief Check if a value is contained.
		 */
		constexpr bool contains( const T p_v ) const
		{
			return std::ranges::any_of( _ranges, [ p_v ]( const auto & r ) { return r.contains( p_v ); } );
		}

		/**
		 * @brief Check if multiple values are contained.
		 */
		constexpr bool contains( const std::initializer_list<T> & p_values ) const
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
		 * @brief Check if a range is contained.
		 */
		constexpr bool contains( const RangeType & p_r ) const
		{
			return std::ranges::any_of(
				_ranges, [ &p_r ]( const auto & r ) { return r.first <= p_r.first && r.last >= p_r.last; } );
		}

		/**
		 * @brief Check if multiple ranges are contained.
		 */
		constexpr bool contains( const std::initializer_list<RangeType> & p_ranges ) const
		{
			for ( const RangeType & range : p_ranges )
			{
				if ( not contains( range ) )
				{
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Check if multiple ranges are contained.
		 */
		constexpr bool contains( const RangeList & p_ranges ) const
		{
			if ( p_ranges.isEmpty() )
			{
				return false;
			}

			for ( auto it = p_ranges.rangeBegin(); it != p_ranges.rangeEnd(); ++it )
			{
				if ( not contains( *it ) )
				{
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Check if a range intersects.
		 */
		constexpr bool intersects( const RangeType & p_other ) const
		{
			for ( const auto & r : _ranges )
			{
				if ( r.intersects( p_other ) )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Check emptiness.
		 */
		constexpr bool isEmpty() const { return _ranges.empty(); }

		/**
		 * @brief Clear all ranges.
		 */
		constexpr void clear() { _ranges.clear(); }

		/**
		 * @brief Get range count.
		 */
		constexpr size_t rangeCount() const { return _ranges.size(); }

		/**
		 * @brief Get all range total count.
		 */
		constexpr T count() const
		{
			T s = 0;
			for ( const auto & r : _ranges )
			{
				s += static_cast<T>( r.getCount() );
			}
			return s;
		}

		/**
		 * @brief Get first value.
		 */
		constexpr T getFirst() const { return _ranges.cbegin()->first; }

		/**
		 * @brief Get last value (exclusive).
		 */
		constexpr T getLast() const { return _ranges.crbegin()->last; }

		/**
		 * @brief Equality operators.
		 */
		friend constexpr bool operator==( const RangeList & a, const RangeList & b )
		{
			if ( a._ranges.size() != b._ranges.size() )
			{
				return false;
			}
			auto ia = a._ranges.begin();
			auto ib = b._ranges.begin();
			for ( ; ia != a._ranges.end(); ++ia, ++ib )
			{
				if ( *ia != *ib )
				{
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Inequality operator.
		 */
		friend constexpr bool operator!=( const RangeList & a, const RangeList & b ) { return not( a == b ); }

		/**
		 * @brief Check if a RangeList is equal to a single range.
		 */
		constexpr bool equals( const RangeType & p_r ) const
		{ return sizeRange() == 1 && rangeBegin()->first == p_r.first && rangeBegin()->last == p_r.last; }

		/**
		 * @brief Merge two RangeLists.
		 */
		static constexpr RangeList merge( const RangeList & p_lhs, const RangeList & p_rhs )
		{
			RangeList res( p_lhs );
			for ( auto it = p_rhs.rangeBegin(); it != p_rhs.rangeEnd(); ++it )
			{
				res.addRange( *it );
			}
			return res;
		}

		/**
		 * @brief Merge another RangeList into this one.
		 */
		constexpr void mergeInPlace( const RangeList & p_rhs )
		{
			for ( auto it = p_rhs.rangeBegin(); it != p_rhs.rangeEnd(); ++it )
			{
				addRange( *it );
			}
		}

		/**
		 * @brief Substract two RangeLists.
		 */
		static constexpr RangeList substract( const RangeList & p_lhs, const RangeList & p_rhs )
		{
			RangeList res( p_lhs );
			for ( auto it = p_rhs.rangeBegin(); it != p_rhs.rangeEnd(); ++it )
			{
				res.removeRange( *it );
			}
			return res;
		}

		/**
		 * @brief Substract another RangeList from this one.
		 */
		constexpr void substractInPlace( const RangeList & p_rhs ) { *this = substract( *this, p_rhs ); }

		/**
		 * @brief Intersect two RangeLists.
		 */
		static constexpr RangeList intersect( const RangeList & p_lhs, const RangeList & p_rhs )
		{
			using R = Range<T>;
			std::vector<R> out;

			auto a = p_lhs.rangeBegin();
			auto b = p_rhs.rangeBegin();

			while ( a != p_lhs.rangeEnd() && b != p_rhs.rangeEnd() )
			{
				if ( a->last <= b->first )
				{
					++a;
					continue;
				}
				if ( b->last <= a->first )
				{
					++b;
					continue;
				}

				const T f = std::max( a->first, b->first );
				const T l = std::min( a->last, b->last );
				if ( f < l )
				{
					out.emplace_back( f, l );
				}

				if ( a->last < b->last )
				{
					++a;
				}
				else if ( a->last > b->last )
				{
					++b;
				}
				else
				{
					++a;
					++b;
				}
			}
			RangeList res;
			res._ranges = std::move( out );
			return res;
		}

		/**
		 * @brief Intersect another RangeList with this one.
		 */
		constexpr void intersectInPlace( const RangeList & p_rhs ) { *this = intersect( *this, p_rhs ); }

		/**
		 * @brief Exclusive two RangeLists.
		 */
		static constexpr RangeList exclusive( const RangeList & p_lhs, const RangeList & p_rhs )
		{ return merge( substract( p_lhs, p_rhs ), substract( p_rhs, p_lhs ) ); }

		/**
		 * @brief Exclusive another RangeList with this one.
		 */
		constexpr void exclusiveInPlace( const RangeList & p_rhs ) { *this = exclusive( *this, p_rhs ); }

		/**
		 * @brief Invert a RangeList in its domain.
		 */
		constexpr RangeList invert() const
		{
			if ( isEmpty() )
			{
				return RangeList {};
			}

			const RangeType domain { getFirst(), getLast() };
			RangeList		full( domain );
			return substract( full, *this );
		}

		/**
		 * @brief Invert in place.
		 */
		constexpr void invertInPlace() { *this = invert(); }

		/**
		 * @brief Generate index range list from container and predicate.
		 */
		/*
		template<class C, class Predicate>
			requires requires( const C & c ) {
				typename C::value_type;
				{ c.size() } -> std::convertible_to<std::size_t>;
				{ c[ std::size_t {} ] } -> std::same_as<const typename C::value_type &>;
			} && std::predicate<Predicate, typename C::value_type>
		static RangeList<typename C::size_type> generateIndexRangeList( const C & container, const Predicate & pred )
		{
			using Idx = typename C::size_type;
			RangeList<Idx> res;
			for ( Idx i = 0; i < container.size(); ++i )
			{
				if ( pred( container[ i ] ) )
				{
					res.addValue( i );
				}
			}
			return res;
		}
		*/

		constexpr size_t sizeRange() const { return _ranges.size(); }

		constexpr size_t size() const
		{
			size_t count = 0;
			for ( const auto & r : _ranges )
			{
				count += r.getCount();
			}
			return count;
		}

		/**
		 * @brief Returns the size in bytes taken by this instance. Heap size and stack size are summed up.
		 */
		constexpr size_t currentSize() const
		{
			size_t totalSize = sizeof( RangeList<T> );
			totalSize += _ranges.size() * sizeof( Range<T> );

			return totalSize;
		}

		/**
		 * @brief To vector of values.
		 */
		template<typename V>
		constexpr std::vector<V> toVector() const
		{
			std::vector<V> res;

			for ( auto it = rangeBegin(); it != rangeEnd(); ++it )
			{
				const V		 first = it->getFirst();
				const size_t count = it->getCount();

				const size_t oldSize = res.size();
				res.resize( oldSize + count );

				std::iota( res.begin() + oldSize, res.end(), first );
			}

			return res;
		}

		/**
		 * @brief String representation.
		 */
		std::string toString() const
		{
			std::string res = "{ ";
			for ( const auto & r : _ranges )
			{
				res += r.toString() + " ";
			}
			res += "}";
			return res;
		}

		/**
		 * @brief Iterators.
		 */
		constexpr std::vector<RangeType>::iterator rangeBegin() { return _ranges.begin(); }

		constexpr std::vector<RangeType>::iterator rangeEnd() { return _ranges.end(); }

		constexpr std::vector<RangeType>::const_iterator rangeBegin() const { return _ranges.begin(); }

		constexpr std::vector<RangeType>::const_iterator rangeEnd() const { return _ranges.end(); }

		/**
		 * @brief Iterate over all values in all ranges.
		 */
		struct iterator
		{
			using ListIt = std::vector<RangeType>::const_iterator;

			ListIt it {};
			ListIt end {};
			T	   cur {};

			struct sentinel
			{
			};

			constexpr void advanceToValid() noexcept
			{
				while ( it != end )
				{
					if ( it->first < it->last )
					{
						cur = it->first;
						return;
					}
					++it;
				}
			}

			constexpr explicit iterator( const std::vector<RangeType> & p_ranges ) noexcept :
				it( p_ranges.begin() ), end( p_ranges.end() )
			{ advanceToValid(); }

			constexpr iterator & operator++() noexcept
			{
				if ( it == end )
				{
					return *this;
				}
				++cur;
				if ( cur >= it->last )
				{
					++it;
					advanceToValid();
				}
				return *this;
			}

			constexpr bool operator!=( sentinel ) const noexcept { return it != end; }

			constexpr T operator*() const noexcept { return cur; }
		};

		constexpr iterator begin() noexcept { return iterator( _ranges ); }

		constexpr iterator::sentinel end() noexcept { return {}; }

		constexpr iterator begin() const noexcept { return iterator( _ranges ); }

		constexpr iterator::sentinel end() const noexcept { return {}; }

	  private:
		/**
		 * @brief All ranges.
		 */
		std::vector<RangeType> _ranges;
	};
} // namespace VTX::Util::Math

#endif
