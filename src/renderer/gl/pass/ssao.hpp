#ifndef __VTX_PASS_SSAO__
#define __VTX_PASS_SSAO__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class SSAO : public BasePass
	{
	  public:
		SSAO( OpenGLFunctions * const p_gl ) : BasePass( p_gl ), _fbo( p_gl ), _texture( p_gl ), _noiseTexture( p_gl )
		{
		}
		virtual ~SSAO() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program *		   _program = nullptr;
		Framebuffer		   _fbo;
		Texture2D		   _texture;
		Texture2D		   _noiseTexture;
		uint			   _kernelSize		 = 16;
		uint			   _noiseTextureSize = 64;
		std::vector<Vec3f> _aoKernel;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
