#ifndef __VTX_PASS_GEOMETRIC__
#define __VTX_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D & getViewPositionsNormalsCompressedTexture() const
		{
			return _viewPositionsNormalsCompressedTexture;
		}
		inline const Texture2D & getColorsTexture() const { return _colorsTexture; }
		inline const Texture2D & getDepthTexture() const { return _depthTexture; }

	  private:
		Framebuffer _fbo								   = Framebuffer();
		Texture2D	_viewPositionsNormalsCompressedTexture = Texture2D();
		Texture2D	_colorsTexture						   = Texture2D();
		Texture2D	_depthTexture						   = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
