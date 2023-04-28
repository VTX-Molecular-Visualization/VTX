#ifndef __VTX_PASS_SHADING__
#define __VTX_PASS_SHADING__

#include "base_pass.hpp"
#include "app/old_app/renderer/gl/framebuffer.hpp"
#include "app/old_app/renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Shading : public BasePass
	{
	  public:
		Shading()		   = default;
		virtual ~Shading() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Application::Scene &, const GL & ) override;
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

} // namespace VTX::Renderer::GL::Pass

#endif
