#ifndef __VTX_RENDERER_BINARY_BUFFER__
#define __VTX_RENDERER_BINARY_BUFFER__

#include "descriptors.hpp"
#include <cstring>
#include <span>
#include <util/math.hpp>
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
	struct LayoutTraitsBase<int>
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
	struct LayoutTraitsBase<Mat3f>
	{
		static constexpr size_t align = 16, size = 48;
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

	using SpanBytes = std::span<const std::byte>;

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
		size_t writeRaw( SpanBytes p_src, const size_t p_align )
		{
			assert( _opened );

			if ( p_src.empty() )
			{
				return _offset;
			}

			const size_t off = writeZero( p_src.size(), p_align );
			std::memcpy( data() + off, p_src.data(), p_src.size() );

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
			constexpr size_t B = sizeof( T );

			assert( _opened );

			const SpanBytes bytes = asBytes( p_v );

			if constexpr ( S == B )
			{
				return writeRaw( bytes, A );
			}
			else
			{
				static_assert( S > B, "LayoutTraits size must be > sizeof(T)" );

				const size_t off = writeZero( S, A );
				std::memcpy( data() + off, bytes.data(), B );
				return off;
			}
		}

		size_t write( const E_TYPE p_type, SpanBytes p_src )
		{
			assert( _opened );

			const size_t elemRaw = rawElementSizeBytes( p_type );

			assert( elemRaw > 0 && "Unsupported E_TYPE for BinaryBuffer::write(E_TYPE, SpanBytes)" );
			assert( ( p_src.size() % elemRaw ) == 0 && "p_src size must be multiple of element raw size" );

			const uint32_t count = static_cast<uint32_t>( p_src.size() / elemRaw );

			size_t firstOffset = 0;
			bool   first	   = true;

			for ( uint32_t i = 0; i < count; ++i )
			{
				const size_t byteOffset = size_t( i ) * elemRaw;
				size_t		 off		= 0;

				switch ( p_type )
				{
				case E_TYPE::BOOL:
				{
					const uint32_t u = _readValue<uint32_t>( p_src, byteOffset );
					off				 = write<uint32_t>( u );
					break;
				}
				case E_TYPE::INT:
				{
					off = write<int32_t>( _readValue<int32_t>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::UINT:
				{
					off = write<uint32_t>( _readValue<uint32_t>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::FLOAT:
				{
					off = write<float>( _readValue<float>( p_src, byteOffset ) );
					break;
				}

				case E_TYPE::VEC2I:
				{
					off = write<Vec2i>( _readValue<Vec2i>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::VEC2F:
				{
					off = write<Vec2f>( _readValue<Vec2f>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::VEC3F:
				{
					off = write<Vec3f>( _readValue<Vec3f>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::VEC4F:
				{
					off = write<Vec4f>( _readValue<Vec4f>( p_src, byteOffset ) );
					break;
				}

				case E_TYPE::MAT3F:
				{
					off = write<Mat3f>( _readValue<Mat3f>( p_src, byteOffset ) );
					break;
				}
				case E_TYPE::MAT4F:
				{
					off = write<Mat4f>( _readValue<Mat4f>( p_src, byteOffset ) );
					break;
				}

				default: assert( false && "Unsupported E_TYPE" ); break;
				}

				if ( first )
				{
					firstOffset = off;
					first		= false;
				}
			}

			return firstOffset;
		}

		/**
		 * @brief Get a span of bytes from a value.
		 */
		template<typename T>
		[[nodiscard]] static SpanBytes asBytes( const T & p_v ) noexcept
		{
			static_assert( std::is_trivially_copyable_v<T>, "BinaryBuffer::asBytes requires trivially copyable type" );

			// std::as_bytes converts a span of T to a span of bytes, modifying the size accordingly.
			return std::as_bytes( std::span<const T, 1>( &p_v, 1 ) );
		}

		/**
		 * @brief Get the raw size in bytes of an element of type E_TYPE.
		 */
		static constexpr size_t rawElementSizeBytes( const E_TYPE p_type )
		{
			switch ( p_type )
			{
			case E_TYPE::BOOL: return sizeof( uint32_t );
			case E_TYPE::INT: return sizeof( int32_t );
			case E_TYPE::UINT: return sizeof( uint32_t );
			case E_TYPE::FLOAT: return sizeof( float );

			case E_TYPE::VEC2I: return sizeof( Vec2i );
			case E_TYPE::VEC2F: return sizeof( Vec2f );
			case E_TYPE::VEC3F: return sizeof( Vec3f );
			case E_TYPE::VEC4F: return sizeof( Vec4f );

			case E_TYPE::MAT3F: return sizeof( Mat3f );
			case E_TYPE::MAT4F: return sizeof( Mat4f );

			default: return 0;
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
			else if constexpr ( L == E_LAYOUT_TYPE::Std430 )
			{
				return Util::Math::alignUp( S, A );
			}
			else
			{
				static_assert( L == E_LAYOUT_TYPE::Std140 || L == E_LAYOUT_TYPE::Std430, "Unsupported layout" );
				return 0;
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

		/**
		 * @brief Conversion to span of bytes.
		 */
		operator std::span<const std::byte>() const noexcept { return { _data.data(), _data.size() }; }

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

		/**
		 * @brief Read a value of type T from a UniformValue at given byte offset.
		 */
		template<typename T>
		static T _readValue( SpanBytes p_src, const size_t p_byteOffset )
		{
			static_assert( std::is_trivially_copyable_v<T> );
			assert( p_byteOffset + sizeof( T ) <= p_src.size() );

			T out {};
			std::memcpy( &out, p_src.data() + p_byteOffset, sizeof( T ) );
			return out;
		}
	};

	/**
	 * @brief Shortcuts.
	 */
	using BinaryBuffer140 = BinaryBuffer<E_LAYOUT_TYPE::Std140>;
	using BinaryBuffer430 = BinaryBuffer<E_LAYOUT_TYPE::Std430>;

} // namespace VTX::Renderer

#endif
