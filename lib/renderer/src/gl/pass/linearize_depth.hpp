#ifndef __VTX_PASS_LINEARIZE_DEPTH__
#define __VTX_PASS_LINEARIZE_DEPTH__

#include "base_pass.hpp"
#include "wrapper/framebuffer.hpp"
#include "wrapper/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class LinearizeDepth : public BasePass
	{
	  public:
		LinearizeDepth()		  = default;
		virtual ~LinearizeDepth() = default;

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;

		inline const Wrapper::Texture2D & getTexture() const { return _texture; }

	  private:
		Program *			 _program = nullptr;
		Wrapper::Framebuffer _fbo	  = Wrapper::Framebuffer();
		Wrapper::Texture2D	 _texture = Wrapper::Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
