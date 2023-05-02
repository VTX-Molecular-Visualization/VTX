#ifndef __VTX_RENDERER_GL_PASS_BLUR__
#define __VTX_RENDERER_GL_PASS_BLUR__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur()			= default;
		virtual ~Blur() = default;

		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		void clearTexture();

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

		Framebuffer _fboFirstPass	  = Framebuffer();
		Texture2D	_textureFirstPass = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
