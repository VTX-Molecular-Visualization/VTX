#ifndef __VTX_RENDERER_GL_PASS_SELECTION__
#define __VTX_RENDERER_GL_PASS_SELECTION__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Selection : public BasePass
	{
	  public:
		Selection( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		virtual ~Selection() = default;

		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		struct StructIn
		{
			Texture2D * textureViewPositionsNormals;
			Texture2D * texture;
			Texture2D * textureDepth;
		} in;

		struct StructOut
		{
			Framebuffer fbo;
			Texture2D	texture;
		} out;

	  private:
		Program * _program;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
