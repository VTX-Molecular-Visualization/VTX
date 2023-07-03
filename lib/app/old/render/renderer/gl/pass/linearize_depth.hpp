#ifndef __VTX_RENDER_RENDERER_GL_PASS_LINEARIZE_DEPTH__
#define __VTX_RENDER_RENDERER_GL_PASS_LINEARIZE_DEPTH__

#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
{
	class LinearizeDepth : public BasePass
	{
	  public:
		LinearizeDepth()		  = default;
		virtual ~LinearizeDepth() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Old::Application::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::App::Old::Render::Renderer::GL::Pass

#endif
