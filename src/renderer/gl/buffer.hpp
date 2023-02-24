#ifndef __VTX_GL_BUFFER_OBJECT__
#define __VTX_GL_BUFFER_OBJECT__

#include "define.hpp"
#include "generic/base_opengl.hpp"

namespace VTX::Renderer::GL
{
	class Buffer : public Generic::BaseOpenGL
	{
	  public:
		// See http://docs.gl/gl4/glBufferData.
		enum class Usage : GLenum
		{
			STREAM_DRAW	 = GL_STREAM_DRAW,
			STREAM_READ	 = GL_STREAM_READ,
			STREAM_COPY	 = GL_STREAM_COPY,
			STATIC_DRAW	 = GL_STATIC_DRAW,
			STATIC_READ	 = GL_STATIC_READ,
			STATIC_COPY	 = GL_STATIC_COPY,
			DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
			DYNAMIC_READ = GL_DYNAMIC_READ,
			DYNAMIC_COPY = GL_DYNAMIC_COPY
		};

		// See https://docs.gl/gl4/glBufferStorage.
		enum class Target : GLenum
		{
			NONE					  = 0,
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

		// See https://docs.gl/gl4/glMapBuffer.
		enum Access : GLbitfield
		{
			READ_ONLY  = GL_READ_ONLY,
			WRITE_ONLY = GL_WRITE_ONLY,
			READ_WRITE = GL_READ_WRITE,
		};

		Buffer() = default;

		template<typename T>
		Buffer( const std::vector<T> & p_vector, const Flags & p_flags = Flags::NONE )
		{
			create();
			set<T>( p_vector, p_flags );
		}

		template<typename T>
		Buffer( const size_t p_size, const T & p_data, const Flags & p_flags = Flags::NONE )
		{
			create();
			set<T>( p_size, p_data, p_flags );
		}

		Buffer( const size_t p_size, const Flags & p_flags = Flags::NONE )
		{
			create();
			set( p_size, p_flags );
		}

		~Buffer() { destroy(); }

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

			_gl->glCreateBuffers( 1, &_id );

			assert( _gl->glIsBuffer( _id ) );
		}

		inline void destroy() const
		{
			if ( _id != GL_INVALID_VALUE )
			{
				_gl->glDeleteBuffers( 1, &_id );
			}
		}

		inline GLuint getId() const { return _id; }

		inline void bind( const Target & p_target )
		{
			assert( _gl->glIsBuffer( _id ) );
			assert( _target == Target::NONE );
			assert( p_target != Target::NONE );

			_target = p_target;
			_gl->glBindBuffer( GLenum( p_target ), _id );
		}

		inline void bind( const Target & p_target, const uint p_index )
		{
			assert( _gl->glIsBuffer( _id ) );
			assert( _target == Target::NONE );
			assert( p_target != Target::NONE );

			_target = p_target;
			_gl->glBindBufferBase( GLenum( _target ), p_index, _id );
		}

		inline void unbind()
		{
			assert( _target != Target::NONE );

			_gl->glBindBuffer( GLenum( _target ), 0 );
			_target = Target::NONE;
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const Usage & p_usage ) const
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glNamedBufferData( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), GLenum( p_usage ) );
		}

		template<typename T>
		inline void setSub( const T & p_data, const uint p_offset, const size_t p_size ) const
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glNamedBufferSubData( _id, GLintptr( p_offset ), GLsizei( p_size ), p_data );
		}

		template<typename T>
		inline void setSub( const std::vector<T> & p_vector ) const
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glNamedBufferSubData( _id, GLintptr( 0 ), GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data() );
		}

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const Flags & p_flags = Flags::NONE )
		{
			assert( _gl->glIsBuffer( _id ) );
			assert( p_vector.empty() == false );

			_gl->glNamedBufferStorage( _id, GLsizei( sizeof( T ) * p_vector.size() ), p_vector.data(), p_flags );
		}

		template<typename T>
		inline void set( const size_t p_size, const T & p_data, const Flags & p_flags = Flags::NONE )
		{
			assert( _gl->glIsBuffer( _id ) );
			assert( p_size > 0 );

			_gl->glNamedBufferStorage( _id, GLsizei( p_size ), &p_data, p_flags );
		}

		inline void set( const size_t p_size, const Flags & p_flags = Flags::NONE )
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glNamedBufferStorage( _id, GLsizei( p_size ), nullptr, p_flags );
		}

		template<typename T>
		inline void const getData( std::vector<T> & p_vector )
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glGetNamedBufferSubData( _id, 0, GLsizei( p_vector.size() * sizeof( T ) ), &p_vector[ 0 ] );
		}

		template<typename T>
		inline void const getData( const uint p_offset, const size_t p_length, T * const p_data )
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glGetNamedBufferSubData( _id, GLintptr( p_offset ), GLsizei( p_length ), p_data );
		}

		template<typename T>
		inline T * const map( const Access & p_access ) const
		{
			assert( _gl->glIsBuffer( _id ) );

			return reinterpret_cast<T *>( _gl->glMapNamedBuffer( _id, p_access ) );
		}

		template<typename T>
		inline T * const map( const uint p_offset, const size_t p_length, const Flags & p_access ) const
		{
			assert( _gl->glIsBuffer( _id ) );

			return reinterpret_cast<T *>(
				_gl->glMapNamedBufferRange( _id, GLintptr( p_offset ), GLsizei( p_length ), p_access ) );
		}

		inline void unmap() const
		{
			assert( _gl->glIsBuffer( _id ) );

			_gl->glUnmapNamedBuffer( _id );
		}

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		Target _target = Target::NONE;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_UNIFORM_BUFFER__
