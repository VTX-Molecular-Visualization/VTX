#ifndef __VTX_RENDERER_GL_TEXTURE_2D__
#define __VTX_RENDERER_GL_TEXTURE_2D__

#include "renderer/gl/include_opengl.hpp"

namespace VTX::Renderer::GL
{
	class Texture2D
	{
	  public:
		Texture2D() = default;
		~Texture2D() { _destroy(); }

		inline int	getId() const { return _id; }
		inline void bindToUnit( const GLuint p_unit ) const { glBindTextureUnit( p_unit, _id ); }

		inline void clear( const void * p_data,
						   const GLenum p_format,
						   const GLenum p_type,
						   const GLint	p_level = 0 ) const
		{
			glClearTexImage( _id, p_level, p_format, p_type, p_data );
		}

		void create( const GLsizei p_width,
					 const GLsizei p_height,
					 const GLenum  p_format	   = GL_RGBA32F,
					 const GLenum  p_wrappingS = GL_REPEAT,
					 const GLenum  p_wrappingT = GL_REPEAT,
					 const GLint   p_minFilter = GL_NEAREST_MIPMAP_LINEAR,
					 const GLint   p_magFilter = GL_LINEAR );

		void resize( const GLsizei p_width, const GLsizei p_height );

		void fill( const void *	 p_pixels,
				   const GLenum	 p_format  = GL_RGB,
				   const GLenum	 p_type	   = GL_FLOAT,
				   const GLint	 p_level   = 0,
				   const GLint	 p_offsetX = 0,
				   const GLint	 p_offsetY = 0,
				   const GLsizei p_width   = -1,
				   const GLsizei p_height  = -1 ) const;

		void getImage( const GLint	 p_level,
					   const GLenum	 p_format,
					   const GLenum	 p_type,
					   const GLsizei p_bufSize,
					   void * const	 p_pixels ) const;

	  private:
		void _create();
		void _destroy();

	  private:
		GLuint _id = GL_INVALID_INDEX;

		GLsizei _width	   = 0;
		GLsizei _height	   = 0;
		GLenum	_format	   = GL_RGBA32F;
		GLenum	_wrappingS = GL_REPEAT;
		GLenum	_wrappingT = GL_REPEAT;
		GLint	_minFilter = GL_NEAREST_MIPMAP_LINEAR;
		GLint	_magFilter = GL_LINEAR;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_TEXTURE_2D__
