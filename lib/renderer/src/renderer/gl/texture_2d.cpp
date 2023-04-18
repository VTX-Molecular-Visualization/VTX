#include "renderer/gl/texture_2d.hpp"
#include <cassert>

namespace VTX::Renderer::GL
{
	void Texture2D::create( const GLsizei p_width,
							const GLsizei p_height,
							const GLenum  p_format,
							const GLenum  p_wrappingS,
							const GLenum  p_wrappingT,
							const GLint	  p_minFilter,
							const GLint	  p_magFilter )

	{
		assert( p_width > 0 && p_height > 0 );

		_width	   = p_width;
		_height	   = p_height;
		_format	   = p_format;
		_wrappingS = p_wrappingS;
		_wrappingT = p_wrappingT;
		_minFilter = p_minFilter;
		_magFilter = p_magFilter;

		_create();
	}

	void Texture2D::resize( const GLsizei p_width, const GLsizei p_height )
	{
		_destroy();
		_width	= p_width;
		_height = p_height;
		_create();
	}

	void Texture2D::fill( const void *	p_pixels,
						  const GLenum	p_format,
						  const GLenum	p_type,
						  const GLint	p_level,
						  const GLint	p_offsetX,
						  const GLint	p_offsetY,
						  const GLsizei p_width,
						  const GLsizei p_height ) const
	{
		const int width	 = p_width == -1 ? _width : p_width;
		const int height = p_height == -1 ? _height : p_height;

		glTextureSubImage2D(
			_id, p_level, p_offsetX, p_offsetY, width, height,  p_format ,  p_type , p_pixels );
	}

	void Texture2D::getImage( const GLint	p_level,
							  const GLenum	p_format,
							  const GLenum	p_type,
							  const GLsizei p_bufSize,
							  void * const	p_pixels ) const
	{
		glGetTextureImage( _id, p_level,  p_format ,  p_type , p_bufSize, p_pixels );
	}

	void Texture2D::_create()
	{
		glCreateTextures( GL_TEXTURE_2D, 1, &_id );
		glTextureParameteri( _id, GL_TEXTURE_WRAP_S, GLint( _wrappingS ) );
		glTextureParameteri( _id, GL_TEXTURE_WRAP_T, GLint( _wrappingT ) );
		glTextureParameteri( _id, GL_TEXTURE_MIN_FILTER, GLint( _minFilter ) );
		glTextureParameteri( _id, GL_TEXTURE_MAG_FILTER, GLint( _magFilter ) );
		glTextureStorage2D( _id, 1,  _format , _width, _height );
	}

	void Texture2D::_destroy() { glDeleteTextures( 1, &_id ); }
} // namespace VTX::Renderer::GL
