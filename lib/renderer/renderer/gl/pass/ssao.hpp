#ifndef __VTX_PASS_SSAO__
#define __VTX_PASS_SSAO__

#include "base_pass.hpp"
#include "gl/program.hpp"
#include "gl/wrapper/framebuffer.hpp"
#include "gl/wrapper/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class SSAO : public BasePass
	{
	  public:
		SSAO()			= default;
		virtual ~SSAO() = default;

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;

		inline const Wrapper::Texture2D & getTexture() const { return _texture; }

	  private:
		Program *			 _program		   = nullptr;
		Wrapper::Framebuffer _fbo			   = Wrapper::Framebuffer();
		Wrapper::Texture2D	 _texture		   = Wrapper::Texture2D();
		Wrapper::Texture2D	 _noiseTexture	   = Wrapper::Texture2D();
		size_t				 _kernelSize	   = 16;
		size_t				 _noiseTextureSize = 64;
		std::vector<Vec3f>	 _aoKernel;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
