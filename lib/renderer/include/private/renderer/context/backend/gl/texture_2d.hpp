#ifndef __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__
#define __VTX_RENDERER_CONTEXT_GL_TEXTURE_2D__

#include "include_opengl.hpp"
#include <cassert>
#include <utility>

namespace VTX::Renderer::Context::Backend::GL
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

		Texture2D( const Texture2D & )			 = delete;
		Texture2D & operator=( const Texture2D & ) = delete;

		Texture2D( Texture2D && p_other ) noexcept
			: _id( std::exchange( p_other._id, GL_INVALID_INDEX ) ),
			  _width( std::exchange( p_other._width, 0 ) ), _height( std::exchange( p_other._height, 0 ) ),
			  _format( std::exchange( p_other._format, GL_RGBA32F ) )
		{
		}

		Texture2D & operator=( Texture2D && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				_destroy();
				_id		= std::exchange( p_other._id, GL_INVALID_INDEX );
				_width	= std::exchange( p_other._width, 0 );
				_height = std::exchange( p_other._height, 0 );
				_format = std::exchange( p_other._format, GL_RGBA32F );
			}

			return *this;
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

		inline void bindToUnit( const GLuint p_index ) const noexcept { glBindTextureUnit( p_index, _id ); }

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

		inline void getSubImage(
			const GLint	  p_level,
			const GLint	  p_xOffset,
			const GLint	  p_yOffset,
			const GLsizei p_width,
			const GLsizei p_height,
			const GLenum  p_format,
			const GLenum  p_type,
			const GLsizei p_bufSize,
			void * const  p_pixels
		) const noexcept
		{
			GLint prev;
			glGetIntegerv( GL_PACK_ALIGNMENT, &prev );
			glPixelStorei( GL_PACK_ALIGNMENT, 1 );
			glGetTextureSubImage(
				_id, p_level, p_xOffset, p_yOffset, 0, p_width, p_height, 1, p_format, p_type, p_bufSize, p_pixels
			);
			glPixelStorei( GL_PACK_ALIGNMENT, prev );
		}

		inline GLsizei getWidth() const noexcept { return _width; }
		inline GLsizei getHeight() const noexcept { return _height; }
		inline GLenum  getFormat() const noexcept { return _format; }

	  private:
		GLuint	_id		= GL_INVALID_INDEX;
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
				glDeleteTextures( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_TEXTURE_2D__
