#ifndef __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__
#define __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__

#include <cassert>
#include <glad/glad.h>

namespace VTX::Renderer::Context::GL
{
	class Texture2D
	{
	  public:
		Texture2D(
			const size_t p_width,
			const size_t p_height,
			const GLenum p_format,
			const GLint	 p_wrappingS,
			const GLint	 p_wrappingT,
			const GLint	 p_minFilter,
			const GLint	 p_magFilter
		)
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

		~Texture2D() { _destroy(); }

		inline int getId() const { return _id; }

		inline void clear( const void * p_data, const GLenum p_format, const GLenum p_type, const GLint p_level = 0 )
			const
		{
			glClearTexImage( _id, p_level, p_format, p_type, p_data );
		}

		inline void resize( const size_t p_width, const size_t p_height )
		{
			_destroy();
			_width	= GLsizei( p_width );
			_height = GLsizei( p_height );
			_create();
		}

		inline void fill(
			const void *  p_pixels,
			const GLenum  p_format	= GL_RGB,
			const GLenum  p_type	= GL_FLOAT,
			const GLint	  p_level	= 0,
			const GLint	  p_offsetX = 0,
			const GLint	  p_offsetY = 0,
			const GLsizei p_width	= -1,
			const GLsizei p_height	= -1
		) const
		{
			const int width	 = p_width == -1 ? _width : p_width;
			const int height = p_height == -1 ? _height : p_height;

			glTextureSubImage2D( _id, p_level, p_offsetX, p_offsetY, width, height, p_format, p_type, p_pixels );
		}

		inline void bind( const GLenum p_target )
		{
			assert( glIsBuffer( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindTexture( p_target, _id );
		}

		inline void bindToUnit( const GLuint p_index ) { glBindTextureUnit( p_index, _id ); }

		inline void unbind()
		{
			assert( _target != 0 );

			glBindTexture( _target, 0 );
			_target = 0;
		}

		inline void unbindFromUnit( const GLuint p_index ) { glBindTextureUnit( p_index, 0 ); }

		inline void getImage(
			const GLint	  p_level,
			const GLenum  p_format,
			const GLenum  p_type,
			const GLsizei p_bufSize,
			void * const  p_pixels
		) const
		{
			glGetTextureImage( _id, p_level, p_format, p_type, p_bufSize, p_pixels );
		}

		inline const GLsizei getWidth() const { return _width; }
		inline const GLsizei getHeight() const { return _height; }
		inline const GLenum	 getFormat() const { return _format; }

	  private:
		GLuint _id	   = GL_INVALID_INDEX;
		GLenum _target = 0;

		GLsizei _width	   = 0;
		GLsizei _height	   = 0;
		GLenum	_format	   = GL_RGBA32F;
		GLint	_wrappingS = GL_REPEAT;
		GLint	_wrappingT = GL_REPEAT;
		GLint	_minFilter = GL_NEAREST_MIPMAP_LINEAR;
		GLint	_magFilter = GL_LINEAR;

		inline void _create()
		{
			glCreateTextures( GL_TEXTURE_2D, 1, &_id );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_S, _wrappingS );
			glTextureParameteri( _id, GL_TEXTURE_WRAP_T, _wrappingT );
			glTextureParameteri( _id, GL_TEXTURE_MIN_FILTER, _minFilter );
			glTextureParameteri( _id, GL_TEXTURE_MAG_FILTER, _magFilter );
			glTextureStorage2D( _id, 1, _format, _width, _height );
		}

		inline void _destroy() { glDeleteTextures( 1, &_id ); }
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_TEXTURE_2D__
