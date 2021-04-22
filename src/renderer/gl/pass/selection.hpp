#ifndef __VTX_PASS_SELECTION__
#define __VTX_PASS_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Selection : public BasePass
	{
	  public:
		Selection( OpenGLFunctions * const p_gl ) : BasePass( p_gl ), _fbo( p_gl ), _texture( p_gl ) {}
		virtual ~Selection() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const GLuint getTexture() const { return _texture.getId(); }
		inline const GLuint getFbo() const { return _fbo.getId(); }

		void updateOutputFBO( const GL & );

	  private:
		Framebuffer _fbo;
		Texture2D	_texture;
		Program *	_program = nullptr;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
