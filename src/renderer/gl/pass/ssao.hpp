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
		SSAO()			= default;
		virtual ~SSAO() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program *		   _program			 = nullptr;
		Framebuffer		   _fbo				 = Framebuffer();
		Texture2D		   _texture			 = Texture2D();
		Texture2D		   _noiseTexture	 = Texture2D();
		uint			   _kernelSize		 = 16;
		uint			   _noiseTextureSize = 64;
		std::vector<Vec3f> _aoKernel;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
