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
		Blur( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		virtual ~Blur() = default;

		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		void clearTexture();

		struct StructIn
		{
			Texture2D * texture;
			Texture2D * textureLinearizeDepth;
		} in;

		struct StructOut
		{
			Framebuffer fbo;
			Texture2D	texture;

		} out;

	  private:
		Program * _program = nullptr;

		Framebuffer _fboFirstPass;
		Texture2D	_textureFirstPass;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
