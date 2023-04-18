#ifndef __VTX_RENDERER_GL_BUFFER__
#define __VTX_RENDERER_GL_BUFFER__

#include "renderer/gl/include_opengl.hpp"
#include <cassert>

namespace VTX::Renderer::GL
{
	class Buffer
	{
	  public:
		Buffer() = default;

		template<typename T>
		Buffer( const std::vector<T> & p_vector, const GLbitfield & p_flags = 0 )
		{
			create();
			set<T>( p_vector, p_flags );
		}

		template<typename T>
		Buffer( const size_t p_size, const T & p_data, const GLbitfield & p_flags = 0 )
		{
			create();
			set<T>( p_size, p_data, p_flags );
		}

		Buffer( const size_t p_size, const GLbitfield & p_flags = 0 )
		{
			create();
			set( p_size, p_flags );
		}

		~Buffer() { destroy(); }

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}

		inline void destroy()
		{
			if ( glIsBuffer( _id ) )
			{
				assert( _target == 0 );

				glDeleteBuffers( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline GLuint getId() const { return _id; }

		inline void bind( const GLenum & p_target )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindBuffer( GLenum( p_target ), _id );
		}

		inline void bind( const GLenum & p_target, const uint p_index )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindBufferBase( GLenum( _target ), p_index, _id );
		}

		inline void unbind()
		{
			assert( _target != 0 );

			glBindBuffer( GLenum( _target ), 0 );
			_target = 0;
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const GLenum & p_usage ) const
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferData( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), GLenum( p_usage ) );
		}

		template<typename T>
		inline void setSub( const T & p_data, const uint p_offset, const size_t p_size ) const
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferSubData( _id, GLintptr( p_offset ), GLsizei( p_size ), p_data );
		}

		template<typename T>
		inline void setSub( const std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferSubData( _id, GLintptr( 0 ), GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data() );
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const GLbitfield & p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_vector.empty() == false );

			glNamedBufferStorage( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_flags );
		}

		template<typename T>
		inline void set( const size_t p_size, const T & p_data, const GLbitfield & p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );

			glNamedBufferStorage( _id, GLsizei( p_size ), &p_data, p_flags );
		}

		inline void set( const size_t p_size, const GLbitfield & p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferStorage( _id, GLsizei( p_size ), nullptr, p_flags );
		}

		template<typename T>
		inline void const getData( std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );

			glGetNamedBufferSubData( _id, 0, GLsizei( p_vector.size() * sizeof( T ) ), &p_vector[ 0 ] );
		}

		template<typename T>
		inline void const getData( const uint p_offset, const size_t p_length, T * const p_data ) const
		{
			assert( glIsBuffer( _id ) );

			glGetNamedBufferSubData( _id, GLintptr( p_offset ), GLsizei( p_length ), p_data );
		}

		template<typename T>
		inline T * const map( const GLbitfield & p_access ) const
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>( glMapNamedBuffer( _id, p_access ) );
		}

		template<typename T>
		inline T * const map( const uint p_offset, const size_t p_length, const GLbitfield & p_access ) const
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>(
				glMapNamedBufferRange( _id, GLintptr( p_offset ), GLsizei( p_length ), p_access ) );
		}

		inline void unmap() const
		{
			assert( glIsBuffer( _id ) );

			glUnmapNamedBuffer( _id );
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
