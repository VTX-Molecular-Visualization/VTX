#ifndef __VTX_RENDERER_GL_PASS_LINEARIZE_DEPTH__
#define __VTX_RENDERER_GL_PASS_LINEARIZE_DEPTH__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class LinearizeDepth : public BasePass
	{
	  public:
		LinearizeDepth()		  = default;
		virtual ~LinearizeDepth() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		// inline const Texture2D & getTexture() const { return _texture; }

	  private:
		std::unique_ptr<Program> _program;
		Framebuffer				 _fbo	  = Framebuffer();
		Texture2D				 _texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
