#ifndef __VTX_RENDER_RENDERER_GL_PASS_OUTLINE__
#define __VTX_RENDER_RENDERER_GL_PASS_OUTLINE__

#include "app/render/renderer/gl/framebuffer.hpp"
#include "app/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Render::Renderer::GL::Pass
{
	class Outline : public BasePass
	{
	  public:
		Outline()		   = default;
		virtual ~Outline() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Application::Scene &, const GL & ) override;

		inline const Texture2D &   getTexture() const { return _texture; }
		inline const Framebuffer & getFbo() const { return _fbo; }

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::App::Render::Renderer::GL::Pass

#endif
