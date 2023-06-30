#ifndef __VTX_RENDERER_GL_PASS_SSAO__
#define __VTX_RENDERER_GL_PASS_SSAO__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"
#include <array>

namespace VTX::Renderer::GL::Pass
{
	class SSAO
	{
	  public:
		SSAO( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		void setIntensity( const float );
		void refreshKernel();

		struct StructIn
		{
			Texture2D * textureDataPacked = nullptr;
			Texture2D * textureDepth	  = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;

		} out;

		struct StructUniforms
		{
			float	 intensity		  = 5.f;
			uint32_t kernelSize		  = 16;
			uint32_t noiseTextureSize = 64;
		} uniforms;

	  private:
		std::unique_ptr<Buffer>	   _ubo;
		std::unique_ptr<Texture2D> _noiseTexture;
		Program *				   _program = nullptr;
	};

	using PassSSAO = BasePass<SSAO>;
} // namespace VTX::Renderer::GL::Pass

#endif
