#ifndef __VTX_RENDERER_GL_PASS_SHADING__
#define __VTX_RENDERER_GL_PASS_SHADING__

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

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;
		// void set();

		// inline const Texture2D &   getTexture() const { return _texture; }
		// inline const Framebuffer & getFbo() const { return _fbo; }

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
