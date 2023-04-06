#ifndef __VTX_PASS_GEOMETRIC__
#define __VTX_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "gl/wrapper/framebuffer.hpp"
#include "gl/wrapper/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;

		inline const Wrapper::Framebuffer & getFbo() const { return _fbo; }
		inline const Wrapper::Texture2D &	getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture;
		}
		inline const Wrapper::Texture2D & getColorsTexture() const { return _colorsTexture; }
		inline const Wrapper::Texture2D & getDepthTexture() const { return _depthTexture; }

	  private:
		Wrapper::Framebuffer _fbo									= Wrapper::Framebuffer();
		Wrapper::Texture2D	 _viewPositionsNormalsCompressedTexture = Wrapper::Texture2D();
		Wrapper::Texture2D	 _colorsTexture							= Wrapper::Texture2D();
		Wrapper::Texture2D	 _depthTexture							= Wrapper::Texture2D();
		Wrapper::Texture2D	 _pickingTexture						= Wrapper::Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
