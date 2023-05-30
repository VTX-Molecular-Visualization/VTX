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
		Shading()		   = default;
		virtual ~Shading() = default;

		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		struct StructIn
		{
			Texture2D * textureViewPositionsNormals = nullptr;
			Texture2D * texture						= nullptr;
			// TODO:check why 2 textures are needed.
			Texture2D * textureBlur = nullptr;
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
