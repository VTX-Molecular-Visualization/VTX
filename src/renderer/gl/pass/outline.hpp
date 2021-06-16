#ifndef __VTX_PASS_OUTLINE__
#define __VTX_PASS_OUTLINE__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Outline : public BasePass
	{
	  public:
		Outline( OpenGLFunctions * const p_gl ) : BasePass( p_gl ), _fbo( p_gl ), _texture( p_gl ) {}
		virtual ~Outline() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D &   getTexture() const { return _texture; }
		inline const Framebuffer & getFbo() const { return _fbo; }

	  private:
		Framebuffer _fbo;
		Texture2D	_texture;
		Program *	_program = nullptr;
	};
} // namespace VTX::Renderer::GL::Pass

#endif
