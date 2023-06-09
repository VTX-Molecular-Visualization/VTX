#ifndef __VTX_PASS_FXAA__
#define __VTX_PASS_FXAA__

#include "base_pass.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class FXAA : public BasePass
	{
	  public:
		FXAA() = default;
		virtual ~FXAA();

		void init( const uint, const uint, const GL & ) override;
		void resize( const uint, const uint, const GL & ) override;
		void render( const Object3D::Scene &, const GL & ) override;

		inline const Texture2D & getTexture() const { return _texture; }

	  private:
		Program * _program = nullptr;
		Texture2D _texture = Texture2D();
	};
} // namespace VTX::Renderer::GL::Pass

#endif
