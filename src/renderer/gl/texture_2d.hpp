#ifndef __VTX_GL_TEXTURE_2D__
#define __VTX_GL_TEXTURE_2D__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"

namespace VTX::Renderer::GL
{
	class Texture2D : public Generic::BaseOpenGL
	{
	  public:
		enum class Type : GLenum
		{
			// See http://docs.gl/gl4/glTexSubImage2D.
			UNSIGNED_BYTE				= GL_UNSIGNED_BYTE,
			BYTE						= GL_BYTE,
			UNSIGNED_SHORT				= GL_UNSIGNED_SHORT,
			SHORT						= GL_SHORT,
			UNSIGNED_INT				= GL_UNSIGNED_INT,
			INT							= GL_INT,
			FLOAT						= GL_FLOAT,
			UNSIGNED_BYTE_3_3_2			= GL_UNSIGNED_BYTE_3_3_2,
			UNSIGNED_BYTE_2_3_3_REV		= GL_UNSIGNED_BYTE_2_3_3_REV,
			UNSIGNED_SHORT_5_6_5		= GL_UNSIGNED_SHORT_5_6_5,
			UNSIGNED_SHORT_5_6_5_REV	= GL_UNSIGNED_SHORT_5_6_5_REV,
			UNSIGNED_SHORT_4_4_4_4		= GL_UNSIGNED_SHORT_4_4_4_4,
			UNSIGNED_SHORT_4_4_4_4_REV	= GL_UNSIGNED_SHORT_4_4_4_4_REV,
			UNSIGNED_SHORT_5_5_5_1		= GL_UNSIGNED_SHORT_5_5_5_1,
			UNSIGNED_SHORT_1_5_5_5_REV	= GL_UNSIGNED_SHORT_1_5_5_5_REV,
			UNSIGNED_INT_8_8_8_8		= GL_UNSIGNED_INT_8_8_8_8,
			UNSIGNED_INT_8_8_8_8_REV	= GL_UNSIGNED_INT_8_8_8_8_REV,
			UNSIGNED_INT_10_10_10_2		= GL_UNSIGNED_INT_10_10_10_2,
			UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV

		};
		enum class Format : GLenum
		{
			// See http://docs.gl/gl4/glTexSubImage2D.
			RED				= GL_RED,
			RG				= GL_RG,
			RGB				= GL_RGB,
			BGR				= GL_BGR,
			BGRA			= GL_BGRA,
			DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
			STENCIL_INDEX	= GL_STENCIL_INDEX
		};
		enum class InternalFormat : GLenum
		{
			// See http://docs.gl/gl4/glTexStorage2D.
			R8	 = GL_R8,
			R16F = GL_R16F,
			R32F = GL_R32F,

			RGB16F	 = GL_RGB16F,
			RGBA16F	 = GL_RGBA16F,
			RGBA32F	 = GL_RGBA32F,
			RGBA32UI = GL_RGBA32UI,

			DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F
			/// TODO: complete
		};
		enum class Wrapping : GLint
		{
			// See http://docs.gl/gl4/glTexParameter.
			CLAMP_TO_EDGE		 = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER		 = GL_CLAMP_TO_BORDER,
			MIRRORED_REPEAT		 = GL_MIRRORED_REPEAT,
			REPEAT				 = GL_REPEAT,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
		};
		enum class Filter : GLint
		{
			// See http://docs.gl/gl4/glTexParameter.
			NEAREST				   = GL_NEAREST,
			LINEAR				   = GL_LINEAR,
			NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_NEAREST
		};

		/// TODO: ca m'emmerde d'avoir ce p_gl dans tous les constructeurs...
		/// Le faire static dans la base ? Besoin de plusieurs contextes ?
		Texture2D( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
		~Texture2D() { _destroy(); }

		void create( const GLsizei		  p_width,
					 const GLsizei		  p_height,
					 const InternalFormat p_format	  = InternalFormat::RGBA32F,
					 const Wrapping		  p_wrappingS = Wrapping::REPEAT,
					 const Wrapping		  p_wrappingT = Wrapping::REPEAT,
					 const Filter		  p_minFilter = Filter::NEAREST_MIPMAP_LINEAR,
					 const Filter		  p_magFilter = Filter::LINEAR );

		inline int getId() const { return _id; }

		void resize( const GLsizei p_width, const GLsizei p_height );

		inline void bindToUnit( const GLuint p_unit ) const { _gl->glBindTextureUnit( p_unit, _id ); }

		void fill( const void *	 p_pixels,
				   const Format	 p_format  = Format::RGB,
				   const Type	 p_type	   = Type::FLOAT,
				   const GLint	 p_level   = 0,
				   const GLint	 p_offsetX = 0,
				   const GLint	 p_offsetY = 0,
				   const GLsizei p_width   = -1,
				   const GLsizei p_height  = -1 ) const;

		inline void clear( const void * p_data,
						   const Format p_format,
						   const Type	p_type,
						   const GLint	p_level = 0 ) const
		{
			_gl->glClearTexImage( _id, p_level, GLenum( p_format ), GLenum( p_type ), p_data );
		}

	  private:
		void _create();
		void _destroy();

	  private:
		GLuint _id = GL_INVALID_INDEX;

		GLsizei		   _width	  = 0;
		GLsizei		   _height	  = 0;
		InternalFormat _format	  = InternalFormat::RGBA32F;
		Wrapping	   _wrappingS = Wrapping::REPEAT;
		Wrapping	   _wrappingT = Wrapping::REPEAT;
		Filter		   _minFilter = Filter::NEAREST_MIPMAP_LINEAR;
		Filter		   _magFilter = Filter::LINEAR;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_TEXTURE_2D__
