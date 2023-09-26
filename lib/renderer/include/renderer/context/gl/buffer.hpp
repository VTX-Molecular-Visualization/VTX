#ifndef __VTX_RENDERER_CONTEXT_GL_BUFFER__
#define __VTX_RENDERER_CONTEXT_GL_BUFFER__

#include <cassert>
#include <cstddef>
#include <glad/glad.h>
#include <vector>

namespace VTX::Renderer::Context::GL::Buffer
{

	inline void create( GLuint * const p_handle )
	{
		glCreateBuffers( 1, p_handle );
		assert( glIsBuffer( *p_handle ) );
	}

	inline void destroy( GLuint * const p_handle )
	{
		assert( glIsBuffer( *p_handle ) );
		glDeleteBuffers( 1, p_handle );
	}

	inline void bind( const GLuint p_handle, const GLenum p_target )
	{
		assert( glIsBuffer( p_handle ) );
		assert( p_target != 0 );
		glBindBuffer( p_target, p_handle );
	}

	inline void bind( const GLuint p_handle, const GLenum p_target, const GLuint p_index )
	{
		assert( glIsBuffer( p_handle ) );
		assert( p_target != 0 );
		glBindBufferBase( p_target, p_index, p_handle );
	}

	inline void unbind( const GLenum p_target ) { glBindBuffer( p_target, 0 ); }

	inline void unbind( const GLenum p_target, const GLuint p_index ) { glBindBufferBase( p_target, p_index, 0 ); }

	template<typename T>
	inline void setData( const GLuint p_handle, const std::vector<T> & p_vector, const GLenum p_usage )
	{
		assert( glIsBuffer( p_handle ) );
		glNamedBufferData( p_handle, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_usage );
	}

	template<typename T>
	inline void setData( const GLuint p_handle, const T & p_data, const GLenum p_usage )
	{
		assert( glIsBuffer( p_handle ) );
		glNamedBufferData( p_handle, GLsizei( sizeof( T ) ), &p_data, p_usage );
	}

	template<typename T>
	inline void setSubData( const GLuint   p_handle,
							const T &	   p_data,
							const GLintptr p_offset = GLintptr( 0 ),
							const GLsizei  p_size	= GLsizei( sizeof( T ) ) )
	{
		assert( glIsBuffer( p_handle ) );
		glNamedBufferSubData( p_handle, p_offset, p_size, &p_data );
	}

	template<typename T>
	inline void setSubData( const GLuint p_handle, const std::vector<T> & p_vector )
	{
		assert( glIsBuffer( p_handle ) );
		glNamedBufferSubData( p_handle, GLintptr( 0 ), GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data() );
	}

	template<typename T>
	inline void setStorage( const GLuint p_handle, const std::vector<T> & p_vector, const GLbitfield p_flags = 0 )
	{
		assert( glIsBuffer( p_handle ) );
		assert( p_vector.empty() == false );
		glNamedBufferStorage( p_handle, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_flags );
	}

	template<typename T>
	inline void setStorage( const GLuint	 p_handle,
							const GLsizei	 p_size,
							const T &		 p_data,
							const GLbitfield p_flags = 0 )
	{
		assert( glIsBuffer( p_handle ) );
		assert( p_size > 0 );
		glNamedBufferStorage( p_handle, p_handle, p_size, &p_data, p_flags );
	}

	inline void setStorage( const GLuint p_handle, const GLsizei p_size, const GLbitfield p_flags = 0 )
	{
		assert( glIsBuffer( p_handle ) );
		glNamedBufferStorage( p_handle, p_size, nullptr, p_flags );
	}

	template<typename T>
	inline void const getData( const GLuint p_handle, std::vector<T> & p_vector )
	{
		assert( glIsBuffer( p_handle ) );
		glGetNamedBufferSubData( p_handle, 0, GLsizei( p_vector.size() * sizeof( T ) ), &p_vector[ 0 ] );
	}

	template<typename T>
	inline void const getData( const GLuint	  p_handle,
							   const GLintptr p_offset,
							   const GLsizei  p_length,
							   T * const	  p_data )
	{
		assert( glIsBuffer( p_handle ) );
		glGetNamedBufferSubData( p_handle, p_offset, p_length, p_data );
	}

	template<typename T>
	inline T * const map( const GLuint p_handle, const GLbitfield p_access )
	{
		assert( glIsBuffer( p_handle ) );
		return reinterpret_cast<T *>( glMapNamedBuffer( p_handle, p_access ) );
	}

	template<typename T>
	inline T * const map( const GLuint	   p_handle,
						  const GLintptr   p_offset,
						  const GLsizei	   p_length,
						  const GLbitfield p_access )
	{
		assert( glIsBuffer( p_handle ) );
		return reinterpret_cast<T *>( glMapNamedBufferRange( p_handle, p_offset, p_length, p_access ) );
	}

	inline void unmap( const GLuint p_handle )
	{
		assert( glIsBuffer( p_handle ) );
		glUnmapNamedBuffer( p_handle );
	}

} // namespace VTX::Renderer::Context::GL::Buffer

#endif
