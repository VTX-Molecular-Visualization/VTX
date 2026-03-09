#ifndef __VTX_UTIL_MATH_BITSET__
#define __VTX_UTIL_MATH_BITSET__

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
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
		explicit BitSet( const size_t p_size ) : _size( p_size ), _data( ( p_size + 63 ) >> 6, 0ull ) {}

		inline void set( const size_t p_i ) noexcept
		{
			assert( p_i < _size );
			_data[ p_i >> 6 ] |= ( 1ull << ( p_i & 63 ) );
		}

		inline void set( const size_t p_i, const bool p_value ) noexcept
		{
			assert( p_i < _size );
			p_value ? set( p_i ) : reset( p_i );
		}

		inline void reset( const size_t p_i ) noexcept
		{
			assert( p_i < _size );
			_data[ p_i >> 6 ] &= ~( 1ull << ( p_i & 63 ) );
		}

		[[nodiscard]] inline bool test( const size_t p_i ) const noexcept
		{
			assert( p_i < _size );
			return ( _data[ p_i >> 6 ] & ( 1ull << ( p_i & 63 ) ) ) != 0ull;
		}

		[[nodiscard]] inline size_t size() const noexcept { return _size; }
		[[nodiscard]] inline bool	empty() const noexcept { return _size == 0; }
		[[nodiscard]] inline size_t dataSize() const noexcept { return _data.size(); }

		inline void clear() noexcept { std::fill( _data.begin(), _data.end(), 0ull ); }

		inline void fill() noexcept
		{
			std::fill( _data.begin(), _data.end(), ~0ull );
			_maskUnusedBits();
		}

		[[nodiscard]] inline bool any() const noexcept
		{
			for ( const uint64_t word : _data )
			{
				if ( word != 0ull )
					return true;
			}
			return false;
		}

		[[nodiscard]] inline bool none() const noexcept { return !any(); }

		[[nodiscard]] inline bool all() const noexcept
		{
			if ( _data.empty() )
				return true;

			const size_t fullWordCount = _size >> 6;
			for ( size_t i = 0; i < fullWordCount; ++i )
			{
				if ( _data[ i ] != ~0ull )
					return false;
			}

			const size_t tailBitCount = _size & 63;
			if ( tailBitCount == 0 )
				return true;

			const uint64_t tailMask = ( 1ull << tailBitCount ) - 1ull;
			return ( _data.back() & tailMask ) == tailMask;
		}

		[[nodiscard]] inline size_t count() const noexcept
		{
			size_t res = 0;
			for ( const uint64_t word : _data )
				res += std::popcount( word );
			return res;
		}

		[[nodiscard]] inline bool equals( const BitSet & p_other ) const noexcept
		{
			return _size == p_other._size && _data == p_other._data;
		}

		[[nodiscard]] inline BitSet merge( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.mergeInPlace( p_other );
			return res;
		}

		inline BitSet & mergeInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
				_data[ i ] |= p_other._data[ i ];

			return *this;
		}

		[[nodiscard]] inline BitSet intersect( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.intersectInPlace( p_other );
			return res;
		}

		inline BitSet & intersectInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
				_data[ i ] &= p_other._data[ i ];

			return *this;
		}

		[[nodiscard]] inline BitSet subtract( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.subtractInPlace( p_other );
			return res;
		}

		inline BitSet & subtractInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
				_data[ i ] &= ~p_other._data[ i ];

			_maskUnusedBits();
			return *this;
		}

		[[nodiscard]] inline BitSet xorWith( const BitSet & p_other ) const
		{
			assert( _size == p_other._size );

			BitSet res( *this );
			res.xorInPlace( p_other );
			return res;
		}

		inline BitSet & xorInPlace( const BitSet & p_other ) noexcept
		{
			assert( _size == p_other._size );

			for ( size_t i = 0; i < _data.size(); ++i )
				_data[ i ] ^= p_other._data[ i ];

			_maskUnusedBits();
			return *this;
		}

		[[nodiscard]] inline BitSet operator|( const BitSet & p_other ) const { return merge( p_other ); }
		[[nodiscard]] inline BitSet operator&( const BitSet & p_other ) const { return intersect( p_other ); }
		[[nodiscard]] inline BitSet operator-( const BitSet & p_other ) const { return subtract( p_other ); }
		[[nodiscard]] inline BitSet operator^( const BitSet & p_other ) const { return xorWith( p_other ); }

		inline BitSet & operator|=( const BitSet & p_other ) noexcept { return mergeInPlace( p_other ); }
		inline BitSet & operator&=( const BitSet & p_other ) noexcept { return intersectInPlace( p_other ); }
		inline BitSet & operator-=( const BitSet & p_other ) noexcept { return subtractInPlace( p_other ); }
		inline BitSet & operator^=( const BitSet & p_other ) noexcept { return xorInPlace( p_other ); }

		[[nodiscard]] inline bool operator==( const BitSet & p_other ) const noexcept { return equals( p_other ); }
		[[nodiscard]] inline bool operator!=( const BitSet & p_other ) const noexcept { return !equals( p_other ); }

		template<typename Func>
		inline void forEachSetBit( Func && p_func ) const noexcept( noexcept( p_func( size_t {} ) ) )
		{
			for ( size_t wordIndex = 0; wordIndex < _data.size(); ++wordIndex )
			{
				uint64_t bits = _data[ wordIndex ];

				while ( bits != 0ull )
				{
					const unsigned bit	 = std::countr_zero( bits );
					const size_t   index = wordIndex * 64 + bit;

					if ( index < _size )
						p_func( index );

					bits &= bits - 1;
				}
			}
		}

		[[nodiscard]] inline const std::vector<uint64_t> & data() const noexcept { return _data; }
		[[nodiscard]] inline std::vector<uint64_t> &	   data() noexcept { return _data; }

	  private:
		inline void _maskUnusedBits() noexcept
		{
			if ( _data.empty() )
				return;

			const size_t usedBitsInLastWord = _size & 63;
			if ( usedBitsInLastWord == 0 )
				return;

			_data.back() &= ( 1ull << usedBitsInLastWord ) - 1ull;
		}

		size_t				  _size = 0;
		std::vector<uint64_t> _data;
	};

} // namespace VTX::Util::Math

#endif
