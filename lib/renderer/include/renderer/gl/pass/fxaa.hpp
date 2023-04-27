#ifndef __VTX_RENDERER_GL_PASS_FXAA__
#define __VTX_RENDERER_GL_PASS_FXAA__

#include "base_pass.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class FXAA : public BasePass
	{
	  public:
		FXAA()			= default;
		virtual ~FXAA() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		// inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program * _program = nullptr;
		Texture2D _texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
