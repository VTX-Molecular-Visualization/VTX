#ifndef __VTX_RENDERER_CONTEXT_GL_BUFFER__
#define __VTX_RENDERER_CONTEXT_GL_BUFFER__

#include <cassert>
#include <cstddef>
#include <glad/glad.h>
#include <iostream>
#include <util/logger.hpp>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	class Buffer
	{
	  public:
		Buffer() { _create(); }

		explicit Buffer(
			const void * const p_data,
			const int32_t	   p_size,
			const bool		   p_immutable = false,
			const uint32_t	   p_flags	   = 0
		)
		{
			_create();
			set( p_data, p_size, p_immutable, p_flags );
		}

		~Buffer() { destroy(); }

		inline uint32_t getId() const { return _id; }

		inline void destroy()
		{
			if ( glIsBuffer( _id ) )
			{
				assert( _target == 0 );

				glDeleteBuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline void bind( const uint32_t p_target )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindBuffer( p_target, _id );
		}

		inline void bind( const uint32_t p_target, const uint32_t p_index )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindBufferBase( _target, p_index, _id );
		}

		inline void unbind()
		{
			assert( _target != 0 );

			glBindBuffer( _target, 0 );
			_target = 0;
		}

		inline void unbind( const uint32_t p_index )
		{
			assert( _target != 0 );

			glBindBufferBase( _target, p_index, 0 );
			_target = 0;
		}

		inline void set(
			const void * const p_data,
			const int32_t	   p_size,
			const bool		   p_immutable = false,
			const uint32_t	   p_flags	   = 0
		)
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );
			assert( _size != p_size );

			_size = p_size;
			if ( p_immutable )
			{
				glNamedBufferStorage( _id, _size, p_data, p_flags );
			}
			else
			{
				glNamedBufferData( _id, _size, p_data, p_flags );
			}
		}

		inline void setSub( const void * const p_data, const int32_t p_size, const int32_t p_offset = 0 ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glNamedBufferSubData( _id, p_offset, p_size, p_data );
		}

		inline void const get( void * const p_data, const int32_t p_size, const int64_t p_offset = 0 ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glGetNamedBufferSubData( _id, p_offset, p_size, p_data );
		}

		inline void * const map( const uint32_t p_access ) const
		{
			assert( glIsBuffer( _id ) );

			return glMapNamedBuffer( _id, p_access );
		}

		inline void * const map( const uint32_t p_access, const int32_t p_length, const int64_t p_offset = 0 ) const
		{
			assert( glIsBuffer( _id ) );

			return glMapNamedBufferRange( _id, p_offset, p_length, p_access );
		}

		inline void unmap() const
		{
			assert( glIsBuffer( _id ) );

			glUnmapNamedBuffer( _id );
		}

		inline const int32_t size() const { return _size; }

	  private:
		uint32_t _id	 = GL_INVALID_INDEX;
		uint32_t _target = 0;
		int32_t	 _size	 = 0;

		inline void _create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
