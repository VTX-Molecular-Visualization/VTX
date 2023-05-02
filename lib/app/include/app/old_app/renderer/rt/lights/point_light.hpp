#ifndef __VTX_RENDERER_RAY_TRACER_POINT_LIGHT__
#define __VTX_RENDERER_RAY_TRACER_POINT_LIGHT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_light.hpp"

namespace VTX
{
	namespace Renderer
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
	} // namespace Renderer
} // namespace VTX

#endif
