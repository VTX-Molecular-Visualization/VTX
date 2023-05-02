#ifndef __VTX_PASS_LINEARIZE_DEPTH__
#define __VTX_PASS_LINEARIZE_DEPTH__

#include "base_pass.hpp"
#include "app/old_app/renderer/gl/framebuffer.hpp"
#include "app/old_app/renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class LinearizeDepth : public BasePass
	{
	  public:
		LinearizeDepth()		  = default;
		virtual ~LinearizeDepth() = default;

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const App::Application::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program *	_program = nullptr;
		Framebuffer _fbo	 = Framebuffer();
		Texture2D	_texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
