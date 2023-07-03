#ifndef __VTX_RENDER_RENDERER_GL_PASS_SSAO__
#define __VTX_RENDER_RENDERER_GL_PASS_SSAO__

#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "base_pass.hpp"

namespace VTX::App::Old::Render::Renderer::GL::Pass
{
	class SSAO : public BasePass
	{
	  public:
		SSAO()			= default;
		virtual ~SSAO() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Old::Application::Scene &, const GL & ) override;

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
} // namespace VTX::App::Old::Render::Renderer::GL::Pass

#endif
