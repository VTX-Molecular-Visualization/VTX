#ifndef __VTX_RENDERER_GL_PASS_BLUR__
#define __VTX_RENDERER_GL_PASS_BLUR__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur()			= default;
		virtual ~Blur() = default;

		void init() override;
		void resize() override;
		void render() override;

		inline const Texture2D & getTexture() const { return _texture; }

		void clearTexture();

	  private:
		Program *	_program		  = nullptr;
		Framebuffer _fboFirstPass	  = Framebuffer();
		Framebuffer _fbo			  = Framebuffer();
		Texture2D	_textureFirstPass = Texture2D();
		Texture2D	_texture		  = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
