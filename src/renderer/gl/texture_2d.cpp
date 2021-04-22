#include "texture_2d.hpp"

namespace VTX::Renderer::GL
{
	void Texture2D::create( const int	   p_width,
							const int	   p_height,
							const Format   p_format,
							const Wrapping p_wrappingS,
							const Wrapping p_wrappingT,
							const Filter   p_minFilter,
							const Filter   p_magFilter )

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

	void Texture2D::resize( const int p_width, const int p_height )
	{
		_destroy();
		_width	= p_width;
		_height = p_height;
		_create();
	}

	void Texture2D::_create()
	{
		_gl->glCreateTextures( GL_TEXTURE_2D, 1, &_id );
		_gl->glTextureParameteri( _id, GL_TEXTURE_WRAP_S, _wrappingS );
		_gl->glTextureParameteri( _id, GL_TEXTURE_WRAP_T, _wrappingT );
		_gl->glTextureParameteri( _id, GL_TEXTURE_MIN_FILTER, _minFilter );
		_gl->glTextureParameteri( _id, GL_TEXTURE_MAG_FILTER, _magFilter );
		_gl->glTextureStorage2D( _id, 1, _format, _width, _height );
	}

	void Texture2D::_destroy() { _gl->glDeleteTextures( 1, &_id ); }
} // namespace VTX::Renderer::GL
