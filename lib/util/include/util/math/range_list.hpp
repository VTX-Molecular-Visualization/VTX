#ifndef __VTX_UTIL_MATH_RANGE_LIST__
#define __VTX_UTIL_MATH_RANGE_LIST__

#include "util/math/range.hpp"
#include <algorithm>
#include <concepts>
#include <list>
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
		RangeList() = default;

		RangeList( const std::initializer_list<RangeType> p_ranges )
		{
			for ( const auto & r : p_ranges )
			{
				addRange( r );
			}
		}

		explicit RangeList( const std::list<RangeType> & p_ranges )
		{
			for ( const auto & r : p_ranges )
			{
				addRange( r );
			}
		}

		explicit RangeList( const std::initializer_list<T> p_values )
		{
			for ( const T v : p_values )
			{
				addValue( v );
			}
		}

		explicit RangeList( std::span<const T> p_values )
		{
			for ( const T v : p_values )
			{
				addRange( v );
			}
		}

		explicit RangeList( const T p_values ) { addValue( p_values ); }

		explicit RangeList( const RangeType & p_range ) { addRange( p_range ); }

		/**
		 * @brief Add a single value.
		 */
		void addValue( const T p_v ) { addRange( RangeType { p_v, static_cast<T>( p_v + 1 ) } ); }

		/**
		 * @brief Add a single range.
		 */
		void addRange( const RangeType & p_r )
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
		void removeValue( const T p_v ) { removeRange( RangeType { p_v, static_cast<T>( p_v + 1 ) } ); }

		/**
		 * @brief Remove a single range.
		 */
		void removeRange( const RangeType & p_r )
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

				RangeType right { p_r.last, it->last };
				it->last = p_r.first;
				it		 = _ranges.insert( std::next( it ), right );
				++it;
			}
		}

		/**
		 * @brief Check if a value is contained.
		 */
		bool contains( const T p_v ) const
		{
			for ( const auto & r : _ranges )
			{
				if ( r.contains( p_v ) )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Check if multiple values are contained.
		 */
		bool contains( const std::initializer_list<T> & p_values ) const
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
		bool contains( const RangeType & p_r ) const
		{
			for ( const auto & r : _ranges )
			{
				if ( r.first <= p_r.first && r.last >= p_r.last )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Check if multiple ranges are contained.
		 */
		bool contains( const std::initializer_list<RangeType> & p_ranges ) const
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
		 * @brief Check if a range intersects.
		 */
		bool intersects( const RangeType & p_other ) const
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
		bool isEmpty() const { return _ranges.empty(); }

		/**
		 * @brief Clear all ranges.
		 */
		void clear() { _ranges.clear(); }

		/**
		 * @brief Get range count.
		 */
		size_t rangeCount() const { return _ranges.size(); }

		/**
		 * @brief Get all range total count.
		 */
		T count() const
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
		T getFirst() const { return _ranges.cbegin()->first; }

		/**
		 * @brief Get last value (exclusive).
		 */
		T getLast() const { return _ranges.crbegin()->last; }

		template<typename T1, typename T2>
		void toStdVectorsFirstLast( std::vector<T1> & p_starts, std::vector<T2> & p_lasts ) const
		{
			p_starts.resize( _ranges.size() );
			p_lasts.resize( _ranges.size() );

			size_t i = 0;
			for ( const auto & r : _ranges )
			{
				p_starts[ i ] = static_cast<T1>( r.first );
				p_lasts[ i ]  = static_cast<T2>( r.last );
				++i;
			}
		}

		/**
		 * @brief  Convert to vectors of starts and counts.
		 */
		template<typename T1, typename T2>
		void toStdVectorsFirstCount( std::vector<T1> & p_firsts, std::vector<T2> & p_counts ) const
		{
			p_firsts.resize( _ranges.size() );
			p_counts.resize( _ranges.size() );

			size_t i = 0;
			for ( const auto & r : _ranges )
			{
				p_firsts[ i ] = static_cast<T1>( r.getFirst() );
				p_counts[ i ] = static_cast<T2>( r.getCount() );
				++i;
			}
		}

		/**
		 * @brief Equality operators.
		 */
		friend bool operator==( const RangeList & a, const RangeList & b )
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
		friend bool operator!=( const RangeList & a, const RangeList & b ) { return not( a == b ); }

		/**
		 * @brief Check if a RangeList is equal to a single range.
		 */
		bool equals( const RangeType & p_r ) const
		{
			return sizeRange() == 1 && rangeBegin()->first == p_r.first && rangeBegin()->last == p_r.last;
		}

		/**
		 * @brief Merge two RangeLists.
		 */
		static RangeList merge( const RangeList & p_lhs, const RangeList & p_rhs )
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
		void mergeInPlace( const RangeList & p_rhs )
		{
			for ( auto it = p_rhs.rangeBegin(); it != p_rhs.rangeEnd(); ++it )
			{
				addRange( *it );
			}
		}

		/**
		 * @brief Substract two RangeLists.
		 */
		static RangeList substract( const RangeList & p_lhs, const RangeList & p_rhs )
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
		void substractInPlace( const RangeList & p_rhs ) { *this = substract( *this, p_rhs ); }

		/**
		 * @brief Intersect two RangeLists.
		 */
		static RangeList intersect( const RangeList & p_lhs, const RangeList & p_rhs )
		{
			using R = Range<T>;
			std::list<R> out;

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
			return RangeList( out );
		}

		/**
		 * @brief Intersect another RangeList with this one.
		 */
		void intersectInPlace( const RangeList & p_rhs ) { *this = intersect( *this, p_rhs ); }

		/**
		 * @brief Exclusive two RangeLists.
		 */
		static RangeList exclusive( const RangeList & p_lhs, const RangeList & p_rhs )
		{
			return merge( substract( p_lhs, p_rhs ), substract( p_rhs, p_lhs ) );
		}

		/**
		 * @brief Exclusive another RangeList with this one.
		 */
		void exclusiveInPlace( const RangeList & p_rhs ) { *this = exclusive( *this, p_rhs ); }

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

		inline size_t sizeRange() const { return _ranges.size(); }

		inline size_t size() const
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
		size_t currentSize() const
		{
			size_t totalSize = sizeof( RangeList<T> );
			totalSize += _ranges.size() * sizeof( Range<T> );

			return totalSize;
		}

		/**
		 * @brief Iterators.
		 */
		std::list<RangeType>::iterator		 rangeBegin() { return _ranges.begin(); }
		std::list<RangeType>::iterator		 rangeEnd() { return _ranges.end(); }
		std::list<RangeType>::const_iterator rangeBegin() const { return _ranges.begin(); }
		std::list<RangeType>::const_iterator rangeEnd() const { return _ranges.end(); }

		/**
		 * @brief Iterate over all values in all ranges.
		 */
		struct iterator
		{
			using ListIt = typename std::list<RangeType>::const_iterator;

			ListIt it {};
			ListIt end {};
			T	   cur {};

			struct sentinel
			{
			};

			void advanceToValid() noexcept
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

			explicit iterator( const std::list<RangeType> & p_ranges ) noexcept :
				it( p_ranges.begin() ), end( p_ranges.end() ), cur {}
			{
				advanceToValid();
			}

			iterator & operator++() noexcept
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

			bool operator!=( sentinel ) const noexcept { return it != end; }
			T	 operator*() const noexcept { return cur; }
		};

		iterator		   begin() noexcept { return iterator( _ranges ); }
		iterator::sentinel end() noexcept { return {}; }
		iterator		   begin() const noexcept { return iterator( _ranges ); }
		iterator::sentinel end() const noexcept { return {}; }

	  private:
		/**
		 * @brief All ranges.
		 */
		std::list<RangeType> _ranges;
	};
} // namespace VTX::Util::Math

#endif
