#ifndef __VTX_GL_BUFFER_DATA__
#define __VTX_GL_BUFFER_DATA__

#include <GL/gl3w.h>

namespace VTX::Renderer::GL::Wrapper
{
	class BufferData
	{
	  public:
		enum class Usage : GLenum
		{
			// See http://docs.gl/gl4/glBufferData.
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
		BufferData() = default;
		~BufferData() { glDeleteBuffers( 1, &_id ); }

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateBuffers( 1, &_id );

			assert( glIsBuffer( _id ) );
		}

		inline GLuint getId() const { return _id; }

		template<typename T>
		inline void set( const std::vector<T> & p_vector, const Usage & p_usage ) const
		{
			glNamedBufferData( _id, sizeof( T ) * GLsizei( p_vector.size() ), p_vector.data(), GLenum( p_usage ) );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL::Wrapper

#endif // __VTX_GL_BUFFER__
