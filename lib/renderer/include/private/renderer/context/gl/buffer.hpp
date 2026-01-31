#ifndef __VTX_RENDERER_CONTEXT_GL_BUFFER__
#define __VTX_RENDERER_CONTEXT_GL_BUFFER__

#include "include_opengl.hpp"
#include <cassert>
#include <cstddef>
#include <iostream>
#include <util/logger.hpp>
#include <vector>

namespace VTX::Renderer::Context::GL
{
	class Buffer
	{
	  public:
		Buffer()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}

		~Buffer() noexcept { destroy(); }

		inline GLuint getId() const noexcept { return _id; }

		inline void destroy() noexcept
		{
			if ( glIsBuffer( _id ) )
			{
				glDeleteBuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline void bind( const GLenum p_target ) const noexcept
		{
			assert( glIsBuffer( _id ) );
			assert( p_target != 0 );

			glBindBuffer( p_target, _id );
		}

		inline void bind( const GLenum p_target, const GLuint p_index ) const noexcept
		{
			assert( glIsBuffer( _id ) );
			assert( p_target != 0 );

			glBindBufferBase( p_target, p_index, _id );
		}

		inline void bind(
			const GLenum	 p_target,
			const GLuint	 p_index,
			const GLsizeiptr p_size,
			const GLintptr	 p_offset = 0
		) const noexcept
		{
			assert( glIsBuffer( _id ) );
			assert( p_target != 0 );
			assert( p_size > 0 );

			glBindBufferRange( p_target, p_index, _id, p_offset, p_size );
		}

		static inline void unbind( GLenum p_target ) noexcept { glBindBuffer( p_target, 0 ); }

		static inline void unbindBase( GLenum p_target, GLuint p_index ) noexcept
		{
			glBindBufferBase( p_target, p_index, 0 );
		}

		static inline void unbindRange( GLenum p_target, GLuint p_index ) noexcept
		{
			glBindBufferRange( p_target, p_index, 0, 0, 0 );
		}

		inline void setData(
			const void * const p_data,
			const GLsizei	   p_size,
			const GLintptr	   p_offset = 0,
			const GLenum	   p_usage	= GL_DYNAMIC_DRAW
		)
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );

			if ( p_offset + p_size <= _size )
			{
				setSub( p_data, p_size, p_offset );
			}
			else
			{
				assert( p_offset == 0 );
				_size = p_size;
				glNamedBufferData( _id, _size, p_data, p_usage );
			}
		}

		inline void setStorage( const void * const p_data, const GLsizei p_size, const GLbitfield p_storageFlags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );

			_size = p_size;

			glNamedBufferStorage( _id, _size, p_data, p_storageFlags );
		}

		inline void setSub(
			const void * const p_data,
			const GLsizeiptr   p_size,
			const GLintptr	   p_offset = 0
		) const noexcept
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glNamedBufferSubData( _id, p_offset, p_size, p_data );
		}

		inline void get( void * const p_data, const GLsizeiptr p_size, const GLintptr p_offset = 0 ) const noexcept
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glGetNamedBufferSubData( _id, p_offset, p_size, p_data );
		}

		inline void * map( const GLbitfield p_access ) const noexcept
		{
			assert( glIsBuffer( _id ) );

			return glMapNamedBuffer( _id, p_access );
		}

		inline void * map(
			const GLbitfield p_access,
			const GLsizeiptr p_length,
			const GLintptr	 p_offset = 0
		) const noexcept
		{
			assert( glIsBuffer( _id ) );

			return glMapNamedBufferRange( _id, p_offset, p_length, p_access );
		}

		inline void unmap() const noexcept
		{
			assert( glIsBuffer( _id ) );

			glUnmapNamedBuffer( _id );
		}

		inline GLsizei size() const noexcept { return _size; }

	  private:
		GLuint	_id	  = GL_INVALID_INDEX;
		GLsizei _size = 0;
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
