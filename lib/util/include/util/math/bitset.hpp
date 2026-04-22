#ifndef __VTX_UTIL_MATH_BITSET__
#define __VTX_UTIL_MATH_BITSET__

#include "util/math/range_list.hpp"
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstdint>
#include <vector>

namespace VTX::Util::Math
{
	/**
	 * @brief A simple bitset implementation based on 64 bit chunks.
	 * //TODO: hierarchical cache
	 */
	class BitSet
	{
	  public:
		using Word = uint64_t;

		/**
		 * @brief Default constructor creates an empty bitset.
		 */
		BitSet() = default;

		/**
		 * @brief Constructs a bitset of p_size bits, all initialized to 0.
		 */
		explicit BitSet( const size_t p_size ) : _size( p_size ), _data( ( p_size + 63 ) >> 6, 0ull ) {}

		/**
		 * @brief Constructs a bitset of p_size bits, all initialized to p_value.
		 */
		BitSet( const size_t p_size, const bool p_value ) : BitSet( p_size )
		{
			if ( p_value )
			{
				fill();
			}
		}

		/**
		 * @brief Sets bit p_i to 1.
		 */
		inline void set( const size_t p_i ) noexcept
		{
			assert( p_i < _size );
			_data[ p_i >> 6 ] |= ( 1ull << ( p_i & 63 ) );
		}

		/**
		 * @brief Sets bit p_i to p_value.
		 */
		inline void set( const size_t p_i, const bool p_value ) noexcept
		{
			assert( p_i < _size );
			p_value ? set( p_i ) : reset( p_i );
		}

		/**
		 * @brief Sets all bits in the range to 1.
		 */
		template<typename T>
		inline void set( const Range<T> & p_range ) noexcept
		{
			const Word first = p_range.first;
			const Word last	 = p_range.last;

			assert( last <= _size );

			if ( first >= last )
			{
				return;
			}

			const size_t firstChunk = first >> 6;
			const size_t lastChunk	= ( last - 1 ) >> 6;

			if ( firstChunk == lastChunk )
			{
				const Word mask
					= ( ~0ull << ( first & 63 ) ) & ( ( last & 63 ) ? ( 1ull << ( last & 63 ) ) - 1ull : ~0ull );
				_data[ firstChunk ] |= mask;
				return;
			}

			_data[ firstChunk ] |= ~0ull << ( first & 63 );
			std::fill( _data.begin() + firstChunk + 1, _data.begin() + lastChunk, ~0ull );
			const size_t tail = last & 63;
			_data[ lastChunk ] |= tail ? ( 1ull << tail ) - 1ull : ~0ull;
		}

		/**
		 * @brief Sets all bits in each range to 1.
		 */
		template<typename T>
		inline void set( const RangeList<T> & p_ranges ) noexcept
		{
			for ( const Range<T> & range : p_ranges )
			{
				set( range );
			}
		}

		/**
		 * @brief Sets bit p_i to 0.
		 */
		inline void reset( const size_t p_i ) noexcept
		{
			assert( p_i < _size );
			_data[ p_i >> 6 ] &= ~( 1ull << ( p_i & 63 ) );
		}

		/**
		 * @brief Sets all bits in the range to 0.
		 */
		template<typename T>
		inline void reset( const Range<T> & p_range ) noexcept
		{
			const Word first = p_range.first;
			const Word last	 = p_range.last;

			assert( last <= _size );

			if ( first >= last )
			{
				return;
			}

			const size_t firstChunk = first >> 6;
			const size_t lastChunk	= ( last - 1 ) >> 6;

			if ( firstChunk == lastChunk )
			{
				const Word mask
					= ( ~0ull << ( first & 63 ) ) & ( ( last & 63 ) ? ( 1ull << ( last & 63 ) ) - 1ull : ~0ull );
				_data[ firstChunk ] &= ~mask;
				return;
			}

			_data[ firstChunk ] &= ~( ~0ull << ( first & 63 ) );
			std::fill( _data.begin() + firstChunk + 1, _data.begin() + lastChunk, 0ull );
			const size_t tail = last & 63;
			_data[ lastChunk ] &= tail ? ~( ( 1ull << tail ) - 1ull ) : 0ull;
		}

		/**
		 * @brief Sets all bits in each range to 0.
		 */
		template<typename T>
		inline void reset( const RangeList<T> & p_ranges ) noexcept
		{
			for ( const Range<T> & range : p_ranges )
			{
				reset( range );
			}
		}

		/**
		 * @brief Returns true if bit p_i is set.
		 */
		[[nodiscard]] inline bool test( const size_t p_i ) const noexcept
		{
			assert( p_i < _size );
			return ( _data[ p_i >> 6 ] & ( 1ull << ( p_i & 63 ) ) ) != 0ull;
		}

		/**
		 * @brief Returns true if all bits in the range are set.
		 */
		template<typename T>
		[[nodiscard]] inline bool test( const Range<T> & p_range ) const noexcept
		{
			const Word first = p_range.first;
			const Word last	 = p_range.last;

			assert( last <= _size );

			if ( first >= last )
			{
				return false;
			}

			const size_t firstChunk = first >> 6;
			const size_t lastChunk	= ( last - 1 ) >> 6;

			if ( firstChunk == lastChunk )
			{
				const Word mask
					= ( ~0ull << ( first & 63 ) ) & ( ( last & 63 ) ? ( 1ull << ( last & 63 ) ) - 1ull : ~0ull );
				return ( _data[ firstChunk ] & mask ) == mask;
			}

			if ( ( _data[ firstChunk ] & ( ~0ull << ( first & 63 ) ) ) != ( ~0ull << ( first & 63 ) ) )
			{
				return false;
			}

			for ( size_t i = firstChunk + 1; i < lastChunk; ++i )
			{
				if ( _data[ i ] != ~0ull )
				{
					return false;
				}
			}

			const size_t tail	  = last & 63;
			const Word	 tailMask = tail ? ( 1ull << tail ) - 1ull : ~0ull;
			return ( _data[ lastChunk ] & tailMask ) == tailMask;
		}

		/**
		 * @brief Returns true if all bits in all ranges are set.
		 */
		template<typename T>
		[[nodiscard]] inline bool test( const RangeList<T> & p_ranges ) const noexcept
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( not test( range ) )
				{
					return false;
				}
			}
			return true;
		}

		/**
		 * @brief Returns true if at least one bit is set.
		 */
		[[nodiscard]] inline bool any() const noexcept
		{
			for ( const Word word : _data )
			{
				if ( word != 0ull )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Returns true if at least one bit in the range is set.
		 */
		template<typename T>
		[[nodiscard]] inline bool any( const Range<T> & p_range ) const noexcept
		{
			const Word first = p_range.first;
			const Word last	 = p_range.last;

			assert( last <= _size );

			if ( first >= last )
			{
				return false;
			}

			const size_t firstChunk = first >> 6;
			const size_t lastChunk	= ( last - 1 ) >> 6;

			if ( firstChunk == lastChunk )
			{
				const Word mask
					= ( ~0ull << ( first & 63 ) ) & ( ( last & 63 ) ? ( 1ull << ( last & 63 ) ) - 1ull : ~0ull );
				return ( _data[ firstChunk ] & mask ) != 0ull;
			}

			if ( _data[ firstChunk ] & ( ~0ull << ( first & 63 ) ) )
			{
				return true;
			}

			for ( size_t i = firstChunk + 1; i < lastChunk; ++i )
			{
				if ( _data[ i ] != 0ull )
				{
					return true;
				}
			}

			const size_t tail	  = last & 63;
			const Word	 tailMask = tail ? ( 1ull << tail ) - 1ull : ~0ull;
			return ( _data[ lastChunk ] & tailMask ) != 0ull;
		}

		/**
		 * @brief Returns true if at least one bit in any of the ranges is set.
		 */
		template<typename T>
		[[nodiscard]] inline bool any( const RangeList<T> & p_ranges ) const noexcept
		{
			for ( const Range<T> & range : p_ranges )
			{
				if ( any( range ) )
				{
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Returns true if no bit is set.
		 */
		[[nodiscard]] inline bool none() const noexcept { return not any(); }

		/**
		 * @brief Returns true if all bits are set.
		 */
		[[nodiscard]] inline bool all() const noexcept
		{
			if ( _data.empty() )
			{
				return true;
			}

			const size_t fullWordCount = _size >> 6;
			for ( size_t i = 0; i < fullWordCount; ++i )
			{
				if ( _data[ i ] != ~0ull )
				{
					return false;
				}
			}

			const size_t tailBitCount = _size & 63;
			if ( tailBitCount == 0 )
			{
				return true;
			}

			const Word tailMask = ( 1ull << tailBitCount ) - 1ull;
			return ( _data.back() & tailMask ) == tailMask;
		}

		/**
		 * @brief Returns the number of set bits.
		 */
		[[nodiscard]] inline size_t count() const noexcept
		{
			size_t res = 0;
			for ( const Word word : _data )
			{
				res += std::popcount( word );
			}
			return res;
		}

		/**
		 * @brief Returns true if both bitsets have the same size and content.
		 */
		[[nodiscard]] inline bool equals( const BitSet & p_other ) const noexcept
		{ return _size == p_other._size && _data == p_other._data; }

		/**
		 * @brief Returns the number of bits.
		 */
		[[nodiscard]] inline size_t size() const noexcept { return _size; }

		/**
		 * @brief Returns true if the bitset has no bits.
		 */
		[[nodiscard]] inline bool empty() const noexcept { return _size == 0; }

		/**
		 * @brief Returns the number of 64-bit words in the internal storage.
		 */
		[[nodiscard]] inline size_t dataSize() const noexcept { return _data.size(); }

		/**
		 * @brief Sets all bits to 0.
		 */
		inline void clear() noexcept { std::fill( _data.begin(), _data.end(), 0ull ); }

		/**
		 * @brief Sets all bits to 1.
		 */
		inline void fill() noexcept
		{
			std::fill( _data.begin(), _data.end(), ~0ull );
			_maskUnusedBits();
		}

		/**
		 * @brief Flips all bits in place.
		 */
		inline void flipInPlace() noexcept
		{
			for ( Word & w : _data )
			{
				w = ~w;
			}
			_maskUnusedBits();
		}

		/**
		 * @brief Returns a new bitset with all bits flipped.
		 */
		[[nodiscard]] inline BitSet operator~() const
		{
			BitSet res( _size );
			for ( size_t i = 0; i < _data.size(); ++i )
			{
				res._data[ i ] = ~_data[ i ];
			}
			res._maskUnusedBits();
			return res;
		}

		/**
		 * @brief Returns the union of this bitset and p_other.
		 */
		[[nodiscard]] inline BitSet merge( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.mergeInPlace( p_other );
			return res;
		}

		/**
		 * @brief Sets this bitset to the union with p_other.
		 */
		inline BitSet & mergeInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
			{
				_data[ i ] |= p_other._data[ i ];
			}

			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset with all bits in the range set.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet merge( const Range<T> & p_range ) const
		{
			BitSet res( *this );
			res.mergeInPlace( p_range );
			return res;
		}

		/**
		 * @brief Sets all bits in the range.
		 */
		template<typename T>
		inline BitSet & mergeInPlace( const Range<T> & p_range ) noexcept
		{
			set( p_range );
			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset with all bits in each range set.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet merge( const RangeList<T> & p_ranges ) const
		{
			BitSet res( *this );
			res.mergeInPlace( p_ranges );
			return res;
		}

		/**
		 * @brief Sets all bits in each range.
		 */
		template<typename T>
		inline BitSet & mergeInPlace( const RangeList<T> & p_ranges ) noexcept
		{
			set( p_ranges );
			return *this;
		}

		/**
		 * @brief Returns the intersection of this bitset and p_other.
		 */
		[[nodiscard]] inline BitSet intersect( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.intersectInPlace( p_other );
			return res;
		}

		/**
		 * @brief Sets this bitset to the intersection with p_other.
		 */
		inline BitSet & intersectInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
			{
				_data[ i ] &= p_other._data[ i ];
			}

			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset keeping only bits within the range.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet intersect( const Range<T> & p_range ) const
		{
			BitSet res( *this );
			res.intersectInPlace( p_range );
			return res;
		}

		/**
		 * @brief Clears all bits outside the range.
		 */
		template<typename T>
		inline BitSet & intersectInPlace( const Range<T> & p_range ) noexcept
		{
			const size_t first = static_cast<size_t>( p_range.first );
			const size_t last  = static_cast<size_t>( p_range.last );

			if ( first >= last )
			{
				clear();
				return *this;
			}

			if ( first > 0 )
			{
				reset( Range<size_t>( 0, first ) );
			}

			if ( last < _size )
			{
				reset( Range<size_t>( last, _size ) );
			}

			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset keeping only bits within any of the ranges.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet intersect( const RangeList<T> & p_ranges ) const
		{
			BitSet res( *this );
			res.intersectInPlace( p_ranges );
			return res;
		}

		/**
		 * @brief Clears all bits that do not fall within any of the ranges.
		 */
		template<typename T>
		inline BitSet & intersectInPlace( const RangeList<T> & p_ranges ) noexcept
		{
			BitSet mask( _size );
			mask.set( p_ranges );
			intersectInPlace( mask );
			return *this;
		}

		/**
		 * @brief Returns this bitset minus the bits set in p_other.
		 */
		[[nodiscard]] inline BitSet subtract( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.subtractInPlace( p_other );
			return res;
		}

		/**
		 * @brief Removes from this bitset all bits set in p_other.
		 */
		inline BitSet & subtractInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
			{
				_data[ i ] &= ~p_other._data[ i ];
			}

			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset with all bits in the range cleared.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet subtract( const Range<T> & p_range ) const
		{
			BitSet res( *this );
			res.subtractInPlace( p_range );
			return res;
		}

		/**
		 * @brief Clears all bits in the range.
		 */
		template<typename T>
		inline BitSet & subtractInPlace( const Range<T> & p_range ) noexcept
		{
			reset( p_range );
			return *this;
		}

		/**
		 * @brief Returns a copy of this bitset with all bits in each range cleared.
		 */
		template<typename T>
		[[nodiscard]] inline BitSet subtract( const RangeList<T> & p_ranges ) const
		{
			BitSet res( *this );
			res.subtractInPlace( p_ranges );
			return res;
		}

		/**
		 * @brief Clears all bits in each range.
		 */
		template<typename T>
		inline BitSet & subtractInPlace( const RangeList<T> & p_ranges ) noexcept
		{
			reset( p_ranges );
			return *this;
		}

		/**
		 * @brief Returns the symmetric difference of this bitset and p_other.
		 */
		[[nodiscard]] inline BitSet xorWith( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.xorInPlace( p_other );
			return res;
		}

		/**
		 * @brief Sets this bitset to the symmetric difference with p_other.
		 */
		inline BitSet & xorInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
			{
				_data[ i ] ^= p_other._data[ i ];
			}

			_maskUnusedBits();
			return *this;
		}

		/**
		 * @brief Union operator.
		 */
		[[nodiscard]] inline BitSet operator|( const BitSet & p_other ) const { return merge( p_other ); }
		/**
		 * @brief Intersection operator.
		 */
		[[nodiscard]] inline BitSet operator&( const BitSet & p_other ) const { return intersect( p_other ); }
		/**
		 * @brief Subtraction operator.
		 */
		[[nodiscard]] inline BitSet operator-( const BitSet & p_other ) const { return subtract( p_other ); }
		/**
		 * @brief Symmetric difference operator.
		 */
		[[nodiscard]] inline BitSet operator^( const BitSet & p_other ) const { return xorWith( p_other ); }

		/**
		 * @brief In-place union operator.
		 */
		inline BitSet & operator|=( const BitSet & p_other ) noexcept { return mergeInPlace( p_other ); }
		/**
		 * @brief In-place intersection operator.
		 */
		inline BitSet & operator&=( const BitSet & p_other ) noexcept { return intersectInPlace( p_other ); }
		/**
		 * @brief In-place subtraction operator.
		 */
		inline BitSet & operator-=( const BitSet & p_other ) noexcept { return subtractInPlace( p_other ); }
		/**
		 * @brief In-place symmetric difference operator.
		 */
		inline BitSet & operator^=( const BitSet & p_other ) noexcept { return xorInPlace( p_other ); }

		/**
		 * @brief Equality operator.
		 */
		[[nodiscard]] inline bool operator==( const BitSet & p_other ) const noexcept { return equals( p_other ); }
		/**
		 * @brief Inequality operator.
		 */
		[[nodiscard]] inline bool operator!=( const BitSet & p_other ) const noexcept { return not equals( p_other ); }

		/**
		 * @brief Calls p_func with the index of each set bit, in ascending order.
		 */
		template<typename Func>
		inline void forEachSetBit( Func && p_func ) const noexcept( noexcept( p_func( size_t {} ) ) )
		{
			for ( size_t wordIndex = 0; wordIndex < _data.size(); ++wordIndex )
			{
				Word bits = _data[ wordIndex ];

				while ( bits != 0ull )
				{
					const unsigned bit	 = std::countr_zero( bits );
					const size_t   index = wordIndex * 64 + bit;

					p_func( index );
					bits &= bits - 1;
				}
			}
		}

		/**
		 * @brief Convert to range list.
		 */
		template<std::integral T = size_t>
		[[nodiscard]] inline RangeList<T> toRangeList() const
		{
			RangeList<T> ranges;
			bool		 hasRange = false;
			T			 first	  = 0;
			T			 last	  = 0;

			forEachSetBit(
				[ & ]( const size_t p_index )
				{
					const T index = static_cast<T>( p_index );
					if ( not hasRange )
					{
						first	 = index;
						last	 = index + T( 1 );
						hasRange = true;
						return;
					}

					if ( index == last )
					{
						++last;
						return;
					}

					ranges.addRange( Range<T>( first, last ) );
					first = index;
					last  = index + T( 1 );
				}
			);

			if ( hasRange )
			{
				ranges.addRange( Range<T>( first, last ) );
			}

			return ranges;
		}

		/**
		 * @brief Returns a read-only reference to the internal word storage.
		 */
		[[nodiscard]] inline const std::vector<Word> & data() const noexcept { return _data; }
		/**
		 * @brief Returns a reference to the internal word storage.
		 */
		[[nodiscard]] inline std::vector<Word> & data() noexcept { return _data; }

		/**
		 * @brief Forward iterator over the indices of set bits, in ascending order.
		 */
		class Iterator
		{
		  public:
			using iterator_category = std::forward_iterator_tag;
			using value_type		= size_t;
			using difference_type	= std::ptrdiff_t;

			Iterator( const std::vector<Word> & p_data, const size_t p_wordIndex ) :
				_data( &p_data ), _wordIndex( p_wordIndex ), _bits( 0ull )
			{
				if ( _wordIndex < _data->size() )
				{
					_bits = ( *_data )[ _wordIndex ];
					_skipEmpty();
				}
			}

			size_t operator*() const noexcept { return _wordIndex * 64 + std::countr_zero( _bits ); }

			Iterator & operator++() noexcept
			{
				_bits &= _bits - 1;
				_skipEmpty();
				return *this;
			}

			bool operator==( const Iterator & p_other ) const noexcept { return _wordIndex == p_other._wordIndex; }
			bool operator!=( const Iterator & p_other ) const noexcept { return _wordIndex != p_other._wordIndex; }

		  private:
			void _skipEmpty() noexcept
			{
				while ( _bits == 0ull )
				{
					++_wordIndex;
					if ( _wordIndex >= _data->size() )
					{
						break;
					}
					_bits = ( *_data )[ _wordIndex ];
				}
			}

			const std::vector<Word> * _data;
			size_t					  _wordIndex;
			Word					  _bits;
		};

		/**
		 * @brief Returns an iterator to the first set bit.
		 */
		[[nodiscard]] inline Iterator begin() const noexcept { return Iterator( _data, 0 ); }
		/**
		 * @brief Returns a past-the-end iterator.
		 */
		[[nodiscard]] inline Iterator end() const noexcept { return Iterator( _data, _data.size() ); }

	  private:
		/**
		 * @brief Clears the unused bits in the last word.
		 */
		inline void _maskUnusedBits() noexcept
		{
			if ( _data.empty() )
			{
				return;
			}

			const size_t usedBitsInLastWord = _size & 63;
			if ( usedBitsInLastWord == 0 )
			{
				return;
			}

			_data.back() &= ( 1ull << usedBitsInLastWord ) - 1ull;
		}

		size_t			  _size = 0;
		std::vector<Word> _data;
	};

} // namespace VTX::Util::Math

#endif
