#ifndef __VTX_RENDERER_CONTEXT_GL_TEXTURE__
#define __VTX_RENDERER_CONTEXT_GL_TEXTURE__

#include "include_opengl.hpp"
#include <cassert>
#include <optional>
#include <utility>

namespace VTX::Renderer::Context::Backend::GL
{
	class Texture
	{
	  public:
		Texture( const GLsizei p_width, const GLsizei p_height, const GLenum p_format, const GLenum p_target ) noexcept
			: _width( p_width ), _height( p_height ), _format( p_format ), _target( p_target )
		{
			assert( p_width > 0 && p_height > 0 );
			assert( p_target == GL_TEXTURE_2D || p_target == GL_TEXTURE_CUBE_MAP );
			_create();
		}

		Texture( const Texture & )			   = delete;
		Texture & operator=( const Texture & ) = delete;

		Texture( Texture && p_other ) noexcept :
			_id( std::exchange( p_other._id, GL_INVALID_INDEX ) ), _width( std::exchange( p_other._width, 0 ) ),
			_height( std::exchange( p_other._height, 0 ) ), _format( std::exchange( p_other._format, GL_RGBA32F ) ),
			_target( std::exchange( p_other._target, GL_TEXTURE_2D ) )
		{
		}

		Texture & operator=( Texture && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				_destroy();
				_id		= std::exchange( p_other._id, GL_INVALID_INDEX );
				_width	= std::exchange( p_other._width, 0 );
				_height = std::exchange( p_other._height, 0 );
				_format = std::exchange( p_other._format, GL_RGBA32F );
				_target = std::exchange( p_other._target, GL_TEXTURE_2D );
			}

			return *this;
		}

		~Texture() noexcept { _destroy(); }

		inline GLuint getId() const noexcept { return _id; }

		inline void clear(
			const void * p_data,
			const GLenum p_format,
			const GLenum p_type,
			const GLint	 p_level = 0
		) const noexcept
		{ glClearTexImage( _id, p_level, p_format, p_type, p_data ); }

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

			if ( _target == GL_TEXTURE_CUBE_MAP )
			{
				glTextureSubImage3D(
					_id, p_level, p_offsetX, p_offsetY, 0, width, height, 6, p_format, p_type, p_pixels
				);
			}
			else
			{
				glTextureSubImage2D( _id, p_level, p_offsetX, p_offsetY, width, height, p_format, p_type, p_pixels );
			}
		}

		inline void bindToUnit( const GLuint p_index ) const noexcept { glBindTextureUnit( p_index, _id ); }

		inline void unbindFromUnit( const GLuint p_index ) const noexcept { glBindTextureUnit( p_index, 0 ); }

		inline void generateMipmaps() const noexcept { glGenerateTextureMipmap( _id ); }

		inline void getImage(
			const GLint	  p_level,
			const GLenum  p_format,
			const GLenum  p_type,
			const GLsizei p_bufSize,
			void * const  p_pixels
		) const noexcept
		{ glGetTextureImage( _id, p_level, p_format, p_type, p_bufSize, p_pixels ); }

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
			assert( _target == GL_TEXTURE_2D );

			GLint previousAlignment;
			glGetIntegerv( GL_PACK_ALIGNMENT, &previousAlignment );
			glPixelStorei( GL_PACK_ALIGNMENT, 1 );
			glGetTextureSubImage(
				_id, p_level, p_xOffset, p_yOffset, 0, p_width, p_height, 1, p_format, p_type, p_bufSize, p_pixels
			);
			glPixelStorei( GL_PACK_ALIGNMENT, previousAlignment );
		}

		inline GLsizei getWidth() const noexcept { return _width; }

		inline GLsizei getHeight() const noexcept { return _height; }

		inline GLenum getFormat() const noexcept { return _format; }

		inline GLenum getTarget() const noexcept { return _target; }

	  private:
		GLuint	_id		= GL_INVALID_INDEX;
		GLsizei _width	= 0;
		GLsizei _height = 0;
		GLenum	_format = GL_RGBA32F;
		GLenum	_target = GL_TEXTURE_2D;

		inline void _create() noexcept
		{
			assert( _width > 0 && _height > 0 );

			GLsizei levels = 1;
			if ( _target == GL_TEXTURE_CUBE_MAP )
			{
				for ( GLsizei size = _width > _height ? _width : _height; size > 1; size /= 2 )
				{
					levels++;
				}
			}

			glCreateTextures( _target, 1, &_id );
			glTextureStorage2D( _id, levels, _format, _width, _height );
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
} // namespace VTX::Renderer::Context::Backend::GL

#endif // __VTX_RENDERER_CONTEXT_GL_TEXTURE__
