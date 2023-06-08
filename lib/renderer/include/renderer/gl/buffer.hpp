#ifndef __VTX_RENDERER_GL_BUFFER__
#define __VTX_RENDERER_GL_BUFFER__

#include "renderer/gl/include_opengl.hpp"
#include <cassert>
#include <cstddef>
#include <vector>

namespace VTX::Renderer::GL
{
	class Buffer
	{
	  public:
		Buffer() = default;

		template<typename T>
		explicit Buffer( const std::vector<T> & p_vector, const GLbitfield p_flags = 0 )
		{
			create();
			set<T>( p_vector, p_flags );
		}

		template<typename T>
		explicit Buffer( const size_t p_size, const T & p_data, const GLbitfield p_flags = 0 )
		{
			create();
			set<T>( GLsizei( p_size ), p_data, p_flags );
		}

		explicit Buffer( const size_t p_size, const GLbitfield p_flags = 0 )
		{
			create();
			set( GLsizei( p_size ), p_flags );
		}

		~Buffer() { destroy(); }

		inline GLuint getId() const { return _id; }

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

#if ( VTX_OPENGL_VERSION == 450 )
			glCreateBuffers( 1, &_id );
#else
			glGenBuffers( 1, &_id );
#endif
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

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const GLenum p_usage ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferData( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_usage );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void set( const T & p_data, const GLenum p_usage ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferData( _id, GLsizei( sizeof( T ) ), &p_data, p_usage );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void setSub( const T &	   p_data,
							const GLintptr p_offset = GLintptr( 0 ),
							const GLsizei  p_size	= GLsizei( sizeof( T ) ) ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferSubData( _id, p_offset, p_size, &p_data );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void setSub( const std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferSubData( _id, GLintptr( 0 ), GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data() );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_vector.empty() == false );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferStorage( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_flags );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void set( const GLsizei p_size, const T & p_data, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );
			assert( p_size > 0 );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferStorage( _id, p_size, &p_data, p_flags );
#else
			// TODO
#endif
		}

		inline void set( const GLsizei p_size, const GLbitfield p_flags = 0 )
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glNamedBufferStorage( _id, p_size, nullptr, p_flags );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void const getData( std::vector<T> & p_vector ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glGetNamedBufferSubData( _id, 0, GLsizei( p_vector.size() * sizeof( T ) ), &p_vector[ 0 ] );
#else
			// TODO
#endif
		}

		template<typename T>
		inline void const getData( const GLintptr p_offset, const GLsizei p_length, T * const p_data ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glGetNamedBufferSubData( _id, p_offset, p_length, p_data );
#else
			// TODO
#endif
		}

		template<typename T>
		inline T * const map( const GLbitfield p_access ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			return reinterpret_cast<T *>( glMapNamedBuffer( _id, p_access ) );
#else
			// TODO
#endif
		}

		template<typename T>
		inline T * const map( const GLintptr p_offset, const GLsizei p_length, const GLbitfield p_access ) const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			return reinterpret_cast<T *>( glMapNamedBufferRange( _id, p_offset, p_length, p_access ) );
#else
			// TODO
#endif
		}

		inline void unmap() const
		{
			assert( glIsBuffer( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glUnmapNamedBuffer( _id );
#else
			// TODO
#endif
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
