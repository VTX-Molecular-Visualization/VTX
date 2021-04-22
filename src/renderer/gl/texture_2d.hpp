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
		enum Format // See http://docs.gl/gl4/glTexStorage2D.
		{
			R8	 = GL_R8,
			R16F = GL_R16F,
			R32F = GL_R32F,

			RGBA16F	 = GL_RGBA16F,
			RGBA32F	 = GL_RGBA16F,
			RGBA32UI = GL_RGBA32UI,

			DEPTH32F = GL_DEPTH_COMPONENT32F
		};
		enum Wrapping // See http://docs.gl/gl4/glTexParameter.
		{
			CLAMP_TO_EDGE		 = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER		 = GL_CLAMP_TO_BORDER,
			MIRRORED_REPEAT		 = GL_MIRRORED_REPEAT,
			REPEAT				 = GL_REPEAT,
			MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
		};
		enum Filter // See http://docs.gl/gl4/glTexParameter.
		{
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

		void create( const int		p_width,
					 const int		p_height,
					 const Format	p_format	= Format::RGBA32F,
					 const Wrapping p_wrappingS = Wrapping::REPEAT,
					 const Wrapping p_wrappingT = Wrapping::REPEAT,
					 const Filter	p_minFilter = Filter::NEAREST_MIPMAP_LINEAR,
					 const Filter	p_magFilter = Filter::LINEAR );

		int getId() const { return _id; }

		void resize( const int p_width, const int p_height );

	  private:
		void _create();
		void _destroy();

	  private:
		GLuint _id = GL_INVALID_INDEX;

		int		 _width		= 0;
		int		 _height	= 0;
		Format	 _format	= Format::RGBA32F;
		Wrapping _wrappingS = Wrapping::REPEAT;
		Wrapping _wrappingT = Wrapping::REPEAT;
		Filter	 _minFilter = Filter::NEAREST_MIPMAP_LINEAR;
		Filter	 _magFilter = Filter::LINEAR;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_TEXTURE_2D__
