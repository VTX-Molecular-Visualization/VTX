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

		struct StructIn
		{
			Texture2D * texture				  = nullptr;
			Texture2D * textureLinearizeDepth = nullptr;
		} in;

		struct StructOut
		{
			Framebuffer fbo		= Framebuffer();
			Texture2D	texture = Texture2D();
		} out;

	  private:
		Program * _program = nullptr;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
