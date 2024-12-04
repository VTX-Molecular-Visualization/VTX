#ifndef __VTX_RENDERER_CONTEXT_GL_BUFFER__
#define __VTX_RENDERER_CONTEXT_GL_BUFFER__

#include "renderer/context/include_opengl.hpp"
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
		Buffer() { _create(); }

		template<typename T>
		explicit Buffer( const std::vector<T> & p_vector, const bool p_immutable = false, const GLbitfield p_flags = 0 )
		{
			_create();
			set<T>( p_vector, p_immutable, p_flags );
		}

		template<typename T>
		explicit Buffer( const T & p_data, const bool p_immutable = false, const GLbitfield p_flags = 0 )
		{
			_create();
			set<T>( p_data, p_immutable, p_flags );
		}

		explicit Buffer( const GLsizei p_size, const bool p_immutable = false, const GLbitfield p_flags = 0 )
		{
			_create();
			set( p_size, p_immutable, p_flags );
		}

		~Buffer() { destroy(); }

		inline GLuint getId() const { return _id; }

		inline void destroy()
		{
			if ( glIsBuffer( _id ) )
			{
				assert( _target == 0 );

				glDeleteBuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline void bind( const GLenum p_target )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindBuffer( p_target, _id );
		}

		inline void bind( const GLenum p_target, const GLuint p_index )
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

		inline void unbind( const GLuint p_index )
		{
			assert( _target != 0 );

			glBindBufferBase( _target, p_index, 0 );
			_target = 0;
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const bool p_immutable = false, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( not p_vector.empty() );

			GLsizei size = GLsizei( sizeof( T ) * p_vector.size() );

			assert( size != _size );

			_size = size;
			if ( p_immutable )
			{
				glNamedBufferStorage( _id, _size, p_vector.data(), p_flags );
			}
			else
			{
				glNamedBufferData( _id, _size, p_vector.data(), p_flags );
			}
		}

		template<typename T>
		inline void set( const T & p_data, const bool p_immutable = false, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );

			GLsizei size = GLsizei( sizeof( T ) );

			assert( size != _size );

			_size = size;
			if ( p_immutable )
			{
				glNamedBufferStorage( _id, _size, &p_data, p_flags );
			}
			else
			{
				glNamedBufferData( _id, _size, &p_data, p_flags );
			}
		}

		inline void set(
			const GLsizei	   p_size,
			const void * const data		   = nullptr,
			const bool		   p_immutable = false,
			const GLbitfield   p_flags	   = 0
		)
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );
			assert( _size != p_size );

			_size = p_size;
			if ( p_immutable )
			{
				glNamedBufferStorage( _id, _size, data, p_flags );
			}
			else
			{
				glNamedBufferData( _id, _size, data, p_flags );
			}
		}

		template<typename T>
		inline void setSub(
			const T &	   p_data,
			const GLintptr p_offset = GLintptr( 0 ),
			const GLsizei  p_size	= GLsizei( sizeof( T ) )
		) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glNamedBufferSubData( _id, p_offset, p_size, &p_data );
		}

		template<typename T>
		inline void setSub( const std::vector<T> & p_vector, const GLintptr p_offset = GLintptr( 0 ) ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );

			GLsizei size = GLsizei( sizeof( T ) * p_vector.size() );

			assert( p_offset + size <= _size );

			glNamedBufferSubData( _id, p_offset, size, p_vector.data() );
		}

		template<typename T>
		inline void const get( std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );

			GLsizei size = GLsizei( p_vector.size() * sizeof( T ) );

			assert( size <= _size );

			glGetNamedBufferSubData( _id, 0, size, &p_vector[ 0 ] );
		}

		template<typename T>
		inline void const get( const GLintptr p_offset, const GLsizei p_size, T * const p_data ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );
			assert( p_offset + p_size <= _size );

			glGetNamedBufferSubData( _id, p_offset, p_size, p_data );
		}

		template<typename T>
		inline T * const map( const GLbitfield p_access ) const
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>( glMapNamedBuffer( _id, p_access ) );
		}

		template<typename T>
		inline T * const map( const GLintptr p_offset, const GLsizei p_length, const GLbitfield p_access ) const
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>( glMapNamedBufferRange( _id, p_offset, p_length, p_access ) );
		}

		inline void unmap() const
		{
			assert( glIsBuffer( _id ) );

			glUnmapNamedBuffer( _id );
		}

		inline const GLsizei size() const { return _size; }

	  private:
		GLuint	_id		= GL_INVALID_INDEX;
		GLenum	_target = 0;
		GLsizei _size	= 0;

		inline void _create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
