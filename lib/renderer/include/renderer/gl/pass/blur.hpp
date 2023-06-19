#ifndef __VTX_RENDERER_GL_PASS_BLUR__
#define __VTX_RENDERER_GL_PASS_BLUR__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur
	{
	  public:
		Blur( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		void someMethod() {}
		int	 anotherMethod() { return 42; }

		void clearTexture();

		struct StructIn
		{
			Texture2D * textureColor = nullptr;
			Texture2D * textureDepth = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;

		} out;

	  private:
		Program * _program = nullptr;

		std::unique_ptr<Framebuffer> _fboFirstPass;
		std::unique_ptr<Texture2D>	 _textureFirstPass;
	};

	using PassBlur = BasePass<Blur>;
} // namespace VTX::Renderer::GL::Pass

#endif
