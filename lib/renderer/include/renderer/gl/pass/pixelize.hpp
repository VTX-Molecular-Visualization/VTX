#ifndef __VTX_RENDERER_GL_PASS_PIXELIZE__
#define __VTX_RENDERER_GL_PASS_PIXELIZE__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Pixelize
	{
	  public:
		Pixelize( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		struct StructIn
		{
			Texture2D * textureDataPacked = nullptr;
			Texture2D * textureColor	  = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;
		} out;

	  private:
		Program * _program = nullptr;
	};

	using PassPixelize = BasePass<Pixelize>;
} // namespace VTX::Renderer::GL::Pass

#endif
