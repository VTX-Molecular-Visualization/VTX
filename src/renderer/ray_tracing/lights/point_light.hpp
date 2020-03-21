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
			PointLight( const Vec3f & p_position, const Vec3f & p_color, const float p_power ) :
				BaseLight( p_color, p_power ), _position( p_position )
			{
			}

			Vec3f sample( const Vec3f & p_point ) const override
			{
				// This is not normalized to use length in integrator if need
				return _position - p_point;
			}

			// TODO: back to private !
		  public:
			Vec3f _position;
		};
	} // namespace Renderer
} // namespace VTX

#endif
