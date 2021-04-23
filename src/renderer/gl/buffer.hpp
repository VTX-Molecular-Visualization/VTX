#ifndef __VTX_GL_BUFFER__
#define __VTX_GL_BUFFER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"

namespace VTX::Renderer::GL
{
	class Buffer : public Generic::BaseOpenGL
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
		Buffer( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
		~Buffer() { _gl->glDeleteBuffers( 1, &_id ); }

		void create() { _gl->glCreateBuffers( 1, &_id ); }

		GLuint getId() const { return _id; }

		template<typename T>
		void set( const std::vector<T> & p_vector, const Usage & p_usage ) const
		{
			_gl->glNamedBufferData( _id, sizeof( T ) * GLsizei( p_vector.size() ), p_vector.data(), GLenum( p_usage ) );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_BUFFER__
