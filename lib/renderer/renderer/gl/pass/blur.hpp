#ifndef __VTX_PASS_BLUR__
#define __VTX_PASS_BLUR__

#include "base_pass.hpp"
#include "gl/program.hpp"
#include "gl/wrapper/framebuffer.hpp"
#include "gl/wrapper/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Blur : public BasePass
	{
	  public:
		Blur()			= default;
		virtual ~Blur() = default;

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;

		inline const Wrapper::Texture2D & getTexture() const { return _texture; }

		void clearTexture();

	  private:
		Program *			 _program		   = nullptr;
		Wrapper::Framebuffer _fboFirstPass	   = Wrapper::Framebuffer();
		Wrapper::Framebuffer _fbo			   = Wrapper::Framebuffer();
		Wrapper::Texture2D	 _textureFirstPass = Wrapper::Texture2D();
		Wrapper::Texture2D	 _texture		   = Wrapper::Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
