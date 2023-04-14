#ifndef __VTX_RENDERER_RAY_TRACER_DIRECTIONAL_LIGHT__
#define __VTX_RENDERER_RAY_TRACER_DIRECTIONAL_LIGHT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_light.hpp"

namespace VTX
{
	namespace Renderer
	{
		class DirectionalLight : public BaseLight
		{
		  public:
			DirectionalLight( const Vec3f & p_direction, const Color::Rgba & p_color ) :
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
	} // namespace Renderer
} // namespace VTX

#endif
