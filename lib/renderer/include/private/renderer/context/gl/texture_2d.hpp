#ifndef __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__
#define __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__

#include "include_opengl.hpp"
#include <cassert>

namespace VTX::Renderer::Context::GL
{
	class Texture2D
	{
	  public:
		Texture2D( const GLsizei p_width, const GLsizei p_height, const GLenum p_format ) noexcept
		{
			assert( p_width > 0 && p_height > 0 );

			_width	= p_width;
			_height = p_height;
			_format = p_format;

			_create();
		}

		~Texture2D() noexcept { _destroy(); }

		inline GLuint getId() const noexcept { return _id; }

		inline void clear(
			const void * p_data,
			const GLenum p_format,
			const GLenum p_type,
			const GLint	 p_level = 0
		) const noexcept
		{
			glClearTexImage( _id, p_level, p_format, p_type, p_data );
		}

		inline void resize( const GLsizei p_width, const GLsizei p_height ) noexcept
		{
			assert( p_width > 0 && p_height > 0 );

			_destroy();
			_width	= p_width;
			_height = p_height;
			_create();
		}

		inline void fill(
			const void *				 p_pixels,
			const GLenum				 p_format,
			const GLenum				 p_type,
			const std::optional<GLsizei> p_width   = std::nullopt,
			const std::optional<GLsizei> p_height  = std::nullopt,
			const GLint					 p_level   = 0,
			const GLint					 p_offsetX = 0,
			const GLint					 p_offsetY = 0
		) const noexcept
		{
			assert( p_pixels != nullptr );

			const GLsizei width	 = p_width.has_value() ? *p_width : _width;
			const GLsizei height = p_height.has_value() ? *p_height : _height;

			glTextureSubImage2D( _id, p_level, p_offsetX, p_offsetY, width, height, p_format, p_type, p_pixels );
		}

		inline void bind( const GLenum p_target ) const noexcept
		{
			assert( glIsTexture( _id ) );
			assert( _target == 0 );
			assert( p_target != 0 );

			_target = p_target;
			glBindTexture( p_target, _id );
		}

		inline void bindToUnit( const GLuint p_index ) const noexcept { glBindTextureUnit( p_index, _id ); }

		inline void unbind() const noexcept
		{
			assert( _target != 0 );

			glBindTexture( _target, 0 );
			_target = 0;
		}

		inline void unbindFromUnit( const GLuint p_index ) const noexcept { glBindTextureUnit( p_index, 0 ); }

		inline void getImage(
			const GLint	  p_level,
			const GLenum  p_format,
			const GLenum  p_type,
			const GLsizei p_bufSize,
			void * const  p_pixels
		) const noexcept
		{
			glGetTextureImage( _id, p_level, p_format, p_type, p_bufSize, p_pixels );
		}

		inline GLsizei getWidth() const noexcept { return _width; }
		inline GLsizei getHeight() const noexcept { return _height; }
		inline GLenum  getFormat() const noexcept { return _format; }

	  private:
		GLuint		   _id	   = GL_INVALID_INDEX;
		mutable GLenum _target = 0;

		GLsizei _width	= 0;
		GLsizei _height = 0;
		GLenum	_format = GL_RGBA32F;

		inline void _create() noexcept
		{
			assert( _width > 0 && _height > 0 );

			glCreateTextures( GL_TEXTURE_2D, 1, &_id );
			glTextureStorage2D( _id, 1, _format, _width, _height );
		}

		inline void _destroy() noexcept
		{
			if ( _id != GL_INVALID_INDEX )
			{
				assert( glIsTexture( _id ) );
				glDeleteTextures( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_TEXTURE_2D__
