#ifndef __VTX_APP_RENDER_RENDERER_RT_DIRECTIONAL_LIGHT__
#define __VTX_APP_RENDER_RENDERER_RT_DIRECTIONAL_LIGHT__

#include "base_light.hpp"

namespace VTX::App::Render::Renderer::RT::Light
{
	class DirectionalLight : public BaseLight
	{
	  public:
		DirectionalLight( const Vec3f & p_direction, const Util::Color::Rgba & p_color ) :
			BaseLight( p_color ), _direction( p_direction )
		{
		}

		LightSample sample( const Vec3f & p_point ) const override
		{
			return LightSample( Util::Math::normalize( _direction ), FLOAT_MAX, _color, 1.f );
		}

	  private:
		Vec3f _direction;
	};
} // namespace VTX::App::Render::Renderer::RT::Light

#endif
