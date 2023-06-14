#ifndef __VTX_RENDER_RENDERER_GL_PASS_BLUR__
#define __VTX_RENDER_RENDERER_GL_PASS_BLUR__

#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur()			= default;
		virtual ~Blur() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Old::Application::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

		void clearTexture();

	  private:
		Program *	_program		  = nullptr;
		Framebuffer _fboFirstPass	  = Framebuffer();
		Framebuffer _fbo			  = Framebuffer();
		Texture2D	_textureFirstPass = Texture2D();
		Texture2D	_texture		  = Texture2D();
	};
} // namespace VTX::App::Old::Render::Renderer::GL::Pass

#endif
