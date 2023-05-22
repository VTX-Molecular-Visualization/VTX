#ifndef __VTX_RENDERER_GL_PASS_SSAO__
#define __VTX_RENDERER_GL_PASS_SSAO__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class SSAO : public BasePass
	{
	  public:
		SSAO( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		virtual ~SSAO() = default;

		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		struct StructIn
		{
			Texture2D * textureViewPositionsNormals;
			Texture2D * textureDepth;
		} in;

		struct StructOut
		{
			Framebuffer fbo;
			Texture2D	texture;

		} out;

	  private:
		Program * _program = nullptr;

		uint			   _kernelSize		 = 16;
		uint			   _noiseTextureSize = 64;
		std::vector<Vec3f> _aoKernel;
		Texture2D		   _noiseTexture;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
