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

		template<typename T>
		explicit Buffer( const std::vector<T> & p_vector, const GLbitfield p_flags = 0 )
		{
			_create();
			setData<T>( p_vector, p_flags );
		}

		template<typename T>
		explicit Buffer( const T & p_data, const GLbitfield p_flags = 0 )
		{
			_create();
			setData<T>( p_data, p_flags );
		}

		template<typename T>
		explicit Buffer( const GLsizei p_size, const T & p_data, const GLbitfield p_flags = 0 )
		{
			_create();
			setStorage<T>( p_size, p_data, p_flags );
		}

		explicit Buffer( const GLsizei p_size, const GLbitfield p_flags = 0 )
		{
			_create();
			setStorage( p_size, p_flags );
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
		inline void setData( const std::vector<T> & p_vector, const GLenum p_usage )
		{
			assert( glIsBuffer( _id ) );
			assert( p_vector.empty() == false );

			_size = GLsizei( sizeof( T ) * p_vector.size() );
			glNamedBufferData( _id, _size, p_vector.data(), p_usage );
		}

		template<typename T>
		inline void setData( const T & p_data, const GLenum p_usage )
		{
			assert( glIsBuffer( _id ) );

			_size = GLsizei( sizeof( T ) );
			glNamedBufferData( _id, _size, &p_data, p_usage );
		}

		template<typename T>
		inline void setData( const GLsizei p_size, const T & p_data, const GLenum p_usage )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );

			_size = p_size;
			glNamedBufferData( _id, _size, &p_data, p_usage );
		}

		inline void setData( const GLsizei p_size, const GLenum p_usage )
		{
			assert( glIsBuffer( _id ) );

			_size = p_size;
			glNamedBufferData( _id, _size, nullptr, p_usage );
		}

		template<typename T>
		inline void setSubData(
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
		inline void setSubData( const std::vector<T> & p_vector, const GLintptr p_offset = GLintptr( 0 ) ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );

			GLsizei size = GLsizei( sizeof( T ) * p_vector.size() );

			assert( size <= _size );

			glNamedBufferSubData( _id, p_offset, size, p_vector.data() );
		}

		template<typename T>
		inline void setStorage( const std::vector<T> & p_vector, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_vector.empty() == false );
			assert( _size == 0 );

			_size = GLsizei( sizeof( T ) * p_vector.size() );
			glNamedBufferStorage( _id, _size, p_vector.data(), p_flags );
		}

		template<typename T>
		inline void setStorage( const GLsizei p_size, const T & p_data, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );
			assert( _size == 0 );

			_size = p_size;
			glNamedBufferStorage( _id, _size, &p_data, p_flags );
		}

		inline void setStorage( const GLsizei p_size, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );
			assert( _size == 0 );

			_size = p_size;
			glNamedBufferStorage( _id, _size, nullptr, p_flags );
		}

		template<typename T>
		inline void const getData( std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );
			assert( _size > 0 );

			GLsizei size = GLsizei( p_vector.size() * sizeof( T ) );

			assert( size <= _size );

			glGetNamedBufferSubData( _id, 0, size, &p_vector[ 0 ] );
		}

		template<typename T>
		inline void const getData( const GLintptr p_offset, const GLsizei p_size, T * const p_data ) const
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

		inline size_t getSize() const { return _size; }

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
