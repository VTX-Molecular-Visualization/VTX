#ifndef __VTX_RENDERER_BINARY_BUFFER__
#define __VTX_RENDERER_BINARY_BUFFER__

#include <cstring>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer
{
	enum struct E_LAYOUT_TYPE : uint8_t
	{
		Std140,
		Std430
	};

	/**
	 * @brief Type traits to get the alignment of a type.
	 */
	template<E_LAYOUT_TYPE L, typename T>
	struct LayoutTraits;

	// Base defaults.
	template<typename T>
	struct LayoutTraitsBase;

	template<>
	struct LayoutTraitsBase<float>
	{
		static constexpr size_t align = 4, size = 4;
	};
	template<>
	struct LayoutTraitsBase<uint>
	{
		static constexpr size_t align = 4, size = 4;
	};
	template<>
	struct LayoutTraitsBase<Vec2f>
	{
		static constexpr size_t align = 8, size = 8;
	};
	template<>
	struct LayoutTraitsBase<Vec2i>
	{
		static constexpr size_t align = 8, size = 8;
	};
	template<>
	struct LayoutTraitsBase<Vec4f>
	{
		static constexpr size_t align = 16, size = 16;
	};
	template<>
	struct LayoutTraitsBase<Mat4f>
	{
		static constexpr size_t align = 16, size = 64;
	};

	// Std140.
	template<typename T>
	struct LayoutTraits<E_LAYOUT_TYPE::Std140, T> : LayoutTraitsBase<T>
	{
	};

	// Std430.
	template<typename T>
	struct LayoutTraits<E_LAYOUT_TYPE::Std430, T> : LayoutTraitsBase<T>
	{
	};

	template<>
	struct LayoutTraits<E_LAYOUT_TYPE::Std140, Vec3f>
	{
		static constexpr size_t align = 16, size = 16;
	};

	template<>
	struct LayoutTraits<E_LAYOUT_TYPE::Std430, Vec3f>
	{
		static constexpr size_t align = 16, size = 12;
	};

	/**
	 * @brief Get the raw pointer of a value.
	 */
	template<typename T>
	constexpr const void * rawPtr( const T & p_v ) noexcept
	{
		return static_cast<const void *>( &p_v );
	}

	inline const void * rawPtr( const Vec2f & p_v ) noexcept { return Util::Math::value_ptr( p_v ); }
	inline const void * rawPtr( const Vec2i & p_v ) noexcept { return Util::Math::value_ptr( p_v ); }
	inline const void * rawPtr( const Vec3f & p_v ) noexcept { return Util::Math::value_ptr( p_v ); }
	inline const void * rawPtr( const Vec4f & p_v ) noexcept { return Util::Math::value_ptr( p_v ); }
	inline const void * rawPtr( const Mat4f & p_v ) noexcept { return Util::Math::value_ptr( p_v ); }

	/**
	 * @brief A buffer to store and align heterogeneous data.
	 */
	template<E_LAYOUT_TYPE L>
	class BinaryBuffer
	{
	  public:
		using value_type = std::byte;

		/**
		 * @brief Align the current offset to p_a.
		 */
		size_t alignTo( const size_t p_a )
		{
			assert( p_a > 0 );
			assert( _opened );

			const size_t pad = ( p_a - ( _offset % p_a ) ) % p_a;
			_data.insert( _data.end(), pad, std::byte { 0 } );
			_offset += pad;

			return _offset;
		}

		/**
		 * @brief Write zeroed data.
		 */
		size_t writeZero( const size_t p_size, const size_t p_align )
		{
			assert( _opened );

			alignTo( p_align );
			const size_t off = _offset;
			_data.resize( size() + p_size, std::byte { 0 } );
			_offset += p_size;

			return off;
		}

		/**
		 * @brief Write raw data.
		 */
		size_t writeRaw( const void * p_src, const size_t p_size, const size_t p_align )
		{
			assert( _opened );

			const size_t off = writeZero( p_size, p_align );
			if ( p_src && p_size )
			{
				std::memcpy( data() + off, p_src, p_size );
			}

			return off;
		}

		/**
		 * @brief Write a value of type T with layout L.
		 */
		template<typename T>
		size_t write( const T & p_v )
		{
			constexpr size_t A = LayoutTraits<L, T>::align;
			constexpr size_t S = LayoutTraits<L, T>::size;

			if constexpr ( std::is_same_v<T, Vec3f> && L == E_LAYOUT_TYPE::Std140 )
			{
				// Reserve 16 bytes, copy 12 (vec3).
				const size_t off = writeZero( S, A ); // S=16.
				std::memcpy( data() + off, rawPtr( p_v ), 12 );
				return off;
			}
			else
			{
				return writeRaw( rawPtr( p_v ), S, A );
			}
		}

		/**
		 * @brief Get the stride of type T with layout L.
		 */
		template<typename T>
		static constexpr size_t strideOf()
		{
			constexpr size_t A = LayoutTraits<L, T>::align;
			constexpr size_t S = LayoutTraits<L, T>::size;

			if constexpr ( L == E_LAYOUT_TYPE::Std140 )
			{
				return ( S + 15 ) & ~size_t( 15 ); // Round to 16.
			}
			else
			{
				return Util::Math::alignUp( S, A );
			}
		}

		/**
		 * @brief Close the buffer and align.
		 */
		void close( const size_t p_align = 16 )
		{
			assert( _opened );

			alignTo( p_align );
			_opened = false;
		}

		/**
		 * @brief Accessors.
		 */
		inline const value_type * data() const { return _data.data(); }
		inline value_type *		  data() { return _data.data(); }
		inline size_t			  size() const { return _data.size(); }
		inline size_t			  offset() const { return _offset; }
		inline bool				  empty() const { return _data.empty(); }

		/**
		 * @brief Iterators.
		 */
		/*
		const value_type * begin() const noexcept { return _data.data(); }
		const value_type * end() const noexcept { return _data.data() + _data.size(); }
		value_type *	   begin() noexcept { return _data.data(); }
		value_type *	   end() noexcept { return _data.data() + _data.size(); }
		*/

	  private:
		/**
		 * @brief Data.
		 */
		std::vector<value_type> _data;

		/**
		 * @brief Current offset in the buffer.
		 */
		size_t _offset = 0;

		/**
		 * @brief State.
		 */
		bool _opened = true;
	};

} // namespace VTX::Renderer

#endif
