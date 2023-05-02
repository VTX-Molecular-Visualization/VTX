#ifndef __VTX_RENDER_RENDERER_GL_PASS_FXAA__
#define __VTX_RENDER_RENDERER_GL_PASS_FXAA__

#include "app/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Render::Renderer::GL::Pass
{
	class FXAA : public BasePass
	{
	  public:
		FXAA() = default;
		virtual ~FXAA();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Application::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program * _program = nullptr;
		Texture2D _texture = Texture2D();
	};
} // namespace VTX::App::Render::Renderer::GL::Pass

#endif
