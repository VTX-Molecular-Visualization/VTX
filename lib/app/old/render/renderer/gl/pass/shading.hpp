#ifndef __VTX_RENDER_RENDERER_GL_PASS_SHADING__
#define __VTX_RENDER_RENDERER_GL_PASS_SHADING__

#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
{
	class Shading : public BasePass
	{
	  public:
		Shading()		   = default;
		virtual ~Shading() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Old::Application::Scene &, const GL & ) override;
		void set();

		inline const Texture2D &   getTexture() const { return _texture; }
		inline const Framebuffer & getFbo() const { return _fbo; }

	  private:
		Program * _diffuseShading = nullptr;
		Program * _glossyShading  = nullptr;
		Program * _toonShading	  = nullptr;
		Program * _flatShading	  = nullptr;

		Program * _currentShading = nullptr;

		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};

} // namespace VTX::App::Old::Render::Renderer::GL::Pass

#endif
