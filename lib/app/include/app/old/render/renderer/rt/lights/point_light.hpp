#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_POINT_LIGHT__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_POINT_LIGHT__

#include "base_light.hpp"

namespace VTX::App::Old::Render::Renderer::RT::Light
{
	class PointLight : public BaseLight
	{
	  public:
		PointLight( const Vec3f & p_position, const Util::Color::Rgba & p_color, const float p_power ) :
			BaseLight( p_color, p_power ), _position( p_position )
		{
		}

		LightSample sample( const Vec3f & p_point ) const override
		{
			const Vec3f dir	  = _position - p_point;
			const float dist2 = Util::Math::dot( dir, dir );
			const float dist  = sqrtf( dist2 );

			return LightSample( Util::Math::normalize( dir ), dist, ( _color * _power ) / dist2, _pdf );
		}

	  private:
		Vec3f _position;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Light

#endif
