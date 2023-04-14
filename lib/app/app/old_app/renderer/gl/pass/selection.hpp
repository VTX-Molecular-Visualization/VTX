#ifndef __VTX_PASS_SELECTION__
#define __VTX_PASS_SELECTION__

#include "base_pass.hpp"
#include "old_app/renderer/gl/framebuffer.hpp"
#include "old_app/renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Selection : public BasePass
	{
	  public:
		Selection()			 = default;
		virtual ~Selection() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D &   getTexture() const { return _texture; }
		inline const Framebuffer & getFbo() const { return _fbo; }

		void updateOutputFBO( const GL & );

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
