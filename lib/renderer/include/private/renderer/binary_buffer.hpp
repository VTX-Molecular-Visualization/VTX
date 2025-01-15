#ifndef __VTX_RENDERER_BINARY_BUFFER__
#define __VTX_RENDERER_BINARY_BUFFER__

#include <util/color/rgba.hpp>
#include <vector>

namespace VTX::Renderer
{
	/**
	 * @brief A buffer to store and align heterogeneous data.
	 * // TODO: abstract this class to be used in other contexts.
	 */
	class BinaryBuffer
	{
	  public:
		/**
		 * @brief Write data adn padding at the current offset.
		 */
		template<typename T>
		void write( const T & p_value )
		{
			assert( _opened );

			const size_t size	   = sizeof( T );
			const size_t alignment = TypeAlignment<T>::value;
			const size_t padding   = ( alignment - ( _offset % alignment ) ) % alignment;

			_data.insert( _data.end(), padding + size, 0 );

			_offset += padding;
			std::memcpy( &_data[ _offset ], &p_value, size );
			_offset += size;
		}

		/**
		 * @brief Align to 16 bytes.
		 */
		void close()
		{
			assert( _opened );

			const size_t padding = ( 16 - ( _offset % 16 ) ) % 16;
			_data.insert( _data.end(), padding, 0 );

			_opened = false;
		}

		inline const void * data() const { return _data.data(); }
		inline size_t		size() const { return _data.size(); }
		inline size_t		offset() const { return _offset; }

	  private:
		std::vector<uint8_t> _data;
		bool				 _opened = true;
		size_t				 _offset = 0;

		template<typename T>
		struct TypeAlignment;

		template<>
		struct TypeAlignment<bool>
		{
			static constexpr size_t value = 4;
		};

		template<>
		struct TypeAlignment<int>
		{
			static constexpr size_t value = 4;
		};

		template<>
		struct TypeAlignment<uint>
		{
			static constexpr size_t value = 4;
		};

		template<>
		struct TypeAlignment<float>
		{
			static constexpr size_t value = 4;
		};

		template<>
		struct TypeAlignment<Vec2i>
		{
			static constexpr size_t value = 8;
		};

		template<>
		struct TypeAlignment<Vec2f>
		{
			static constexpr size_t value = 8;
		};

		template<>
		struct TypeAlignment<Vec3f>
		{
			static constexpr size_t value = 16;
		};

		template<>
		struct TypeAlignment<Vec4f>
		{
			static constexpr size_t value = 16;
		};

		template<>
		struct TypeAlignment<Mat4f>
		{
			static constexpr size_t value = 16;
		};
	};

} // namespace VTX::Renderer

#endif
