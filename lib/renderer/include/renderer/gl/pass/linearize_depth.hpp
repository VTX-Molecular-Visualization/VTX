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

		struct StructIn
		{
			Texture2D * textureDepth = nullptr;
		} in;

		struct StructOut
		{
			Framebuffer fbo		= Framebuffer();
			Texture2D	texture = Texture2D();
		} out;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
