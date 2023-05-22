#ifndef __VTX_RENDERER_GL_PASS_SHADING__
#define __VTX_RENDERER_GL_PASS_SHADING__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Shading : public BasePass
	{
	  public:
		Shading( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		virtual ~Shading() = default;

		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		struct StructIn
		{
			Texture2D * textureViewPositionsNormals;
			Texture2D * texture;
			// TODO:check why 2 textures are needed.
			Texture2D * textureBlur = nullptr;
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
