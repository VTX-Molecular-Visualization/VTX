#ifndef __VTX_RENDERER_GL_PASS_OUTLINE__
#define __VTX_RENDERER_GL_PASS_OUTLINE__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Outline : public BasePass
	{
	  public:
		Outline()		   = default;
		virtual ~Outline() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		// inline const Texture2D &   getTexture() const { return _texture; }
		// inline const Framebuffer & getFbo() const { return _fbo; }

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
