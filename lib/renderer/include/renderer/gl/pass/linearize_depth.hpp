#ifndef __VTX_RENDERER_GL_PASS_LINEARIZE_DEPTH__
#define __VTX_RENDERER_GL_PASS_LINEARIZE_DEPTH__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class LinearizeDepth
	{
	  public:
		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		struct StructIn
		{
			Texture2D * textureDepth = nullptr;
		} in;

		struct StructOut
		{
			Framebuffer fbo		= Framebuffer();
			Texture2D	texture = Texture2D();
		} out;

	  private:
		Program * _program = nullptr;
	};

	using PassLinearizeDepth = BasePass<LinearizeDepth>;
} // namespace VTX::Renderer::GL::Pass

#endif
