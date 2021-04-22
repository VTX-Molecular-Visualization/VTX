#ifndef __VTX_PASS_LINEARIZE_DEPTH__
#define __VTX_PASS_LINEARIZE_DEPTH__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class LinearizeDepth : public BasePass
	{
	  public:
		LinearizeDepth( OpenGLFunctions * const p_gl ) : BasePass( p_gl ), _fbo( p_gl ), _texture( p_gl ) {}
		virtual ~LinearizeDepth() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint getTexture() const { return _texture.getId(); }

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo;
		Texture2D	_texture;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
