#ifndef __VTX_PASS_FXAA__
#define __VTX_PASS_FXAA__

#include "base_pass.hpp"
#include "wrapper/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class FXAA : public BasePass
	{
	  public:
		FXAA() = default;
		virtual ~FXAA();

		void init( const size_t, const size_t, const GL & ) override;
		void resize( const size_t, const size_t, const GL & ) override;
		void render( const GL & ) override;

		inline const Wrapper::Texture2D & getTexture() const { return _texture; }

	  private:
		Program *		   _program = nullptr;
		Wrapper::Texture2D _texture = Wrapper::Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
