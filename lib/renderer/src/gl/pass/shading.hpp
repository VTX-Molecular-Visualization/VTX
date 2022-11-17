#ifndef __VTX_PASS_SHADING__
#define __VTX_PASS_SHADING__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Shading : public BasePass
	{
	  public:
		Shading()		   = default;
		virtual ~Shading() = default;

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;
		void set();

		inline const Wrapper::Texture2D &	getTexture() const { return _texture; }
		inline const Wrapper::Framebuffer & getFbo() const { return _fbo; }

	  private:
		Program * _diffuseShading = nullptr;
		Program * _glossyShading  = nullptr;
		Program * _toonShading	  = nullptr;
		Program * _flatShading	  = nullptr;

		Program * _currentShading = nullptr;

		Wrapper::Framebuffer _fbo	  = Wrapper::Framebuffer();
		Wrapper::Texture2D	 _texture = Wrapper::Texture2D();
	};

} // namespace VTX::Renderer::GL::Pass

#endif
