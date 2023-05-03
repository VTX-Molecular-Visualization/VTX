#ifndef __VTX_RENDER_RENDERER_GL_PASS_SELECTION__
#define __VTX_RENDER_RENDERER_GL_PASS_SELECTION__

#include "app/render/renderer/gl/framebuffer.hpp"
#include "app/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Render::Renderer::GL::Pass
{
	class Selection : public BasePass
	{
	  public:
		Selection()			 = default;
		virtual ~Selection() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Application::Scene &, const GL & ) override;

		inline const Texture2D &   getTexture() const { return _texture; }
		inline const Framebuffer & getFbo() const { return _fbo; }

		void updateOutputFBO( const GL & );

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::App::Render::Renderer::GL::Pass

#endif
