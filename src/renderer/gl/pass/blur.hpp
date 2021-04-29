#ifndef __VTX_PASS_BLUR__
#define __VTX_PASS_BLUR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur( OpenGLFunctions * const p_gl ) :
			BasePass( p_gl ), _fboFirstPass( p_gl ), _fbo( p_gl ), _textureFirstPass( p_gl ), _texture( p_gl )
		{
		}
		virtual ~Blur() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

		void clearTexture();

	  private:
		Program *	_program = nullptr;
		Framebuffer _fboFirstPass;
		Framebuffer _fbo;
		Texture2D	_textureFirstPass;
		Texture2D	_texture;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
