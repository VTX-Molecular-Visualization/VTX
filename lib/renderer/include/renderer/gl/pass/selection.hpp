#ifndef __VTX_RENDERER_GL_PASS_SELECTION__
#define __VTX_RENDERER_GL_PASS_SELECTION__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Selection : public BasePass
	{
	  public:
		Selection()			 = default;
		virtual ~Selection() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		// inline const Texture2D &   getTexture() const { return _texture; }
		// inline const Framebuffer & getFbo() const { return _fbo; }

		// void updateOutputFBO( const GL & );

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
