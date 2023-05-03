#ifndef __VTX_RENDERER_GL_TEXTURE_2D__
#define __VTX_RENDERER_GL_TEXTURE_2D__

#include "renderer/gl/include_opengl.hpp"
#include <cassert>

namespace VTX::Renderer::GL
{
	class Texture2D
	{
	  public:
		Texture2D() = default;

		~Texture2D() { destroy(); }

		inline void create( const size_t p_width,
							const size_t p_height,
							const GLenum p_format	 = GL_RGBA32F,
							const GLenum p_wrappingS = GL_REPEAT,
							const GLenum p_wrappingT = GL_REPEAT,
							const GLint	 p_minFilter = GL_NEAREST_MIPMAP_LINEAR,
							const GLint	 p_magFilter = GL_LINEAR )
		{
			assert( p_width > 0 && p_height > 0 );

			_width	   = GLsizei( p_width );
			_height	   = GLsizei( p_height );
			_format	   = p_format;
			_wrappingS = p_wrappingS;
			_wrappingT = p_wrappingT;
			_minFilter = p_minFilter;
			_magFilter = p_magFilter;

			_create();
		}

		inline void destroy() { glDeleteTextures( 1, &_id ); }

		inline int getId() const { return _id; }

		inline void clear( const void * p_data,
						   const GLenum p_format,
						   const GLenum p_type,
						   const GLint	p_level = 0 ) const
		{
			glClearTexImage( _id, p_level, p_format, p_type, p_data );
		}

		inline void resize( const size_t p_width, const size_t p_height )
		{
			destroy();
			_width	= GLsizei( p_width );
			_height = GLsizei( p_height );
			_create();
		}

		inline void fill( const void *	p_pixels,
						  const GLenum	p_format  = GL_RGB,
						  const GLenum	p_type	  = GL_FLOAT,
						  const GLint	p_level	  = 0,
						  const GLint	p_offsetX = 0,
						  const GLint	p_offsetY = 0,
						  const GLsizei p_width	  = -1,
						  const GLsizei p_height  = -1 ) const
		{
			const int width	 = p_width == -1 ? _width : p_width;
			const int height = p_height == -1 ? _height : p_height;

			glTextureSubImage2D( _id, p_level, p_offsetX, p_offsetY, width, height, p_format, p_type, p_pixels );
		}

		inline void bindToUnit( const GLuint p_unit ) const { glBindTextureUnit( p_unit, _id ); }

		inline void getImage( const GLint	p_level,
							  const GLenum	p_format,
							  const GLenum	p_type,
							  const GLsizei p_bufSize,
							  void * const	p_pixels ) const
		{
			glGetTextureImage( _id, p_level, p_format, p_type, p_bufSize, p_pixels );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;

		GLsizei _width	   = 0;
		GLsizei _height	   = 0;
		GLenum	_format	   = GL_RGBA32F;
		GLenum	_wrappingS = GL_REPEAT;
		GLenum	_wrappingT = GL_REPEAT;
		GLint	_minFilter = GL_NEAREST_MIPMAP_LINEAR;
		GLint	_magFilter = GL_LINEAR;

		inline void _create()
		{
			glCreateTextures( GL_TEXTURE_2D, 1, &_id );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_S, GLint( _wrappingS ) );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_T, GLint( _wrappingT ) );
			glTextureParameteri( _id, GL_TEXTURE_MIN_FILTER, GLint( _minFilter ) );
			glTextureParameteri( _id, GL_TEXTURE_MAG_FILTER, GLint( _magFilter ) );
			glTextureStorage2D( _id, 1, _format, _width, _height );
		}
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_TEXTURE_2D__
