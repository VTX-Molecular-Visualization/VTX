#ifndef __VTX_GL_BUFFER_STORAGE__
#define __VTX_GL_BUFFER_STORAGE__

#include <GL/gl3w.h>
#include <cassert>
#include <vector>

namespace VTX::Renderer::GL::Wrapper
{
	class BufferStorage
	{
	  public:
		// See https://docs.gl/gl4/glBufferStorage.
		enum class Target : GLenum
		{
			ARRAY_BUFFER			  = GL_ARRAY_BUFFER,
			ATOMIC_COUNTER_BUFFER	  = GL_ATOMIC_COUNTER_BUFFER,
			COPY_READ_BUFFER		  = GL_COPY_READ_BUFFER,
			COPY_WRITE_BUFFER		  = GL_COPY_WRITE_BUFFER,
			DISPATCH_INDIRECT_BUFFER  = GL_DISPATCH_INDIRECT_BUFFER,
			DRAW_INDIRECT_BUFFER	  = GL_DRAW_INDIRECT_BUFFER,
			ELEMENT_ARRAY_BUFFER	  = GL_ELEMENT_ARRAY_BUFFER,
			PIXEL_PACK_BUFFER		  = GL_PIXEL_PACK_BUFFER,
			PIXEL_UNPACK_BUFFER		  = GL_PIXEL_UNPACK_BUFFER,
			QUERY_BUFFER			  = GL_QUERY_BUFFER,
			SHADER_STORAGE_BUFFER	  = GL_SHADER_STORAGE_BUFFER,
			TEXTURE_BUFFER			  = GL_TEXTURE_BUFFER,
			TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
			UNIFORM_BUFFER			  = GL_UNIFORM_BUFFER
		};

		// See https://docs.gl/gl4/glBufferStorage.
		enum Flags : GLbitfield
		{
			NONE				= 0,
			DYNAMIC_STORAGE_BIT = GL_DYNAMIC_STORAGE_BIT,
			MAP_READ_BIT		= GL_MAP_READ_BIT,
			MAP_WRITE_BIT		= GL_MAP_WRITE_BIT,
			MAP_PERSISTENT_BIT	= GL_MAP_PERSISTENT_BIT,
			MAP_COHERENT_BIT	= GL_MAP_COHERENT_BIT,
			CLIENT_STORAGE_BIT	= GL_CLIENT_STORAGE_BIT,
			// Optionals.
			MAP_INVALIDATE_RANGE_BIT  = GL_MAP_INVALIDATE_RANGE_BIT,
			MAP_INVALIDATE_BUFFER_BIT = GL_MAP_INVALIDATE_BUFFER_BIT,
			MAP_FLUSH_EXPLICIT_BIT	  = GL_MAP_FLUSH_EXPLICIT_BIT,
			MAP_UNSYNCHRONIZED_BIT	  = GL_MAP_UNSYNCHRONIZED_BIT
		};

		BufferStorage() = default;

		BufferStorage( const Target & p_target ) { create( p_target ); }

		template<typename T>
		BufferStorage( const Target & p_target, const std::vector<T> & p_vector, const Flags & p_flags = Flags::NONE )
		{
			create( p_target );
			set<T>( p_vector, p_flags );
		}

		template<typename T>
		BufferStorage( const Target & p_target,
					   const size_t	  p_size,
					   const T &	  p_data,
					   const Flags &  p_flags = Flags::NONE )
		{
			create( p_target );
			set<T>( p_size, p_data, p_flags );
		}

		/*
		BufferStorage( const Target & p_target, const size_t p_size, const Flags & p_flags = Flags::NONE )
		{
			create( p_target );
			const std::vector<unsigned int> data = std::vector<unsigned int>( p_size, 0u );
			set<unsigned int>( data, p_flags );
		}
		*/

		~BufferStorage() { destroy(); }

		inline void create( const Target & p_target )
		{
			assert( _id == GL_INVALID_INDEX );

			_target = p_target;
			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}

		inline void destroy()
		{
			if ( _id != GL_INVALID_VALUE )
			{
				glDeleteBuffers( 1, &_id );
			}
		}

		inline GLuint getId() const { return _id; }

		inline void bind() const
		{
			assert( glIsBuffer( _id ) );

			glBindBuffer( GLenum( _target ), _id );
		}

		inline void bind( const Target & p_target ) const
		{
			assert( glIsBuffer( _id ) );

			glBindBuffer( GLenum( p_target ), _id );
		}

		inline void bind( const GLuint p_index ) const
		{
			assert( glIsBuffer( _id ) );

			glBindBufferBase( GLenum( _target ), p_index, _id );
		}

		inline void bind( const GLuint p_index, const Target & p_target ) const
		{
			assert( glIsBuffer( _id ) );

			glBindBufferBase( GLenum( p_target ), p_index, _id );
		}

		inline void unbind() const { glBindBuffer( GLenum( _target ), 0 ); }

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const Flags & p_flags = Flags::NONE ) const
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferStorage( _id, sizeof( T ) * GLsizei( p_vector.size() ), p_vector.data(), p_flags );
		}

		template<typename T>
		inline void set( const GLuint p_size, const T & p_data, const Flags & p_flags = Flags::NONE ) const
		{
			assert( glIsBuffer( _id ) );

			glNamedBufferStorage( _id, GLsizei( p_size ), &p_data, p_flags );
		}

		template<typename T>
		inline void const getData( const GLuint p_offset, const GLuint p_length, T * const p_data )
		{
			assert( glIsBuffer( _id ) );

			glGetNamedBufferSubData( _id, GLintptr( p_offset ), GLsizeiptr( p_length ), p_data );
		}

		template<typename T>
		inline T * const map( const Flags & p_access = Flags::NONE )
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>( glMapNamedBuffer( _id, p_access ) );
		}

		template<typename T>
		inline T * const map( const GLuint p_offset, const GLuint p_length, const Flags & p_access = Flags::NONE )
		{
			assert( glIsBuffer( _id ) );

			return reinterpret_cast<T *>(
				glMapNamedBufferRange( _id, GLintptr( p_offset ), GLsizeiptr( p_length ), p_access ) );
		}

		inline void unmap()
		{
			assert( glIsBuffer( _id ) );

			glUnmapNamedBuffer( _id );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
		Target _target;
	};
} // namespace VTX::Renderer::GL::Wrapper

#endif // __VTX_GL_UNIFORM_BUFFER__
