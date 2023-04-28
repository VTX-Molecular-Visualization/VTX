#ifndef __VTX_RENDERER_RAY_TRACER_QUAD_LIGHT__
#define __VTX_RENDERER_RAY_TRACER_QUAD_LIGHT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_light.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class QuadLight : public BaseLight
		{
		  public:
			QuadLight( const Vec3f &	   p_position,
					   const Vec3f &	   p_u,
					   const Vec3f &	   p_v,
					   const Util::Color::Rgba & p_color,
					   const float		   p_power ) :
				BaseLight( p_color, p_power ),
				_position( p_position ), _u( p_u ), _v( p_v )
			{
				const Vec3f invDir = Util::Math::cross( _v, _u );
				_pdf			   = 1.f / Util::Math::length( invDir );
				_invNormal		   = invDir * _pdf;
				_isSurface		   = true;
			}

			LightSample sample( const Vec3f & p_point ) const override
			{
				const float ru = Util::Math::randomFloat();
				const float rv = Util::Math::randomFloat();

				const Vec3f position  = _position + ru * _u + rv * _v;
				Vec3f		direction = position - p_point;
				const float dist	  = Util::Math::length( direction );
				Util::Math::normalizeSelf( direction );

				const float		  cosDir   = Util::Math::dot( _invNormal, direction );
				const float		  pdf	   = _pdf * ( dist * dist ) / fabsf( cosDir );
				const Util::Color::Rgba radiance = cosDir > 0.f ? _color * _power / pdf : Util::Color::Rgba::BLACK;

				return LightSample( direction, dist, radiance, pdf );
			}

		  private:
			Vec3f _position	 = VEC3F_ZERO; // corner position
			Vec3f _u		 = VEC3F_X;	   // vectors adjacent to the corner
			Vec3f _v		 = VEC3F_Z;
			Vec3f _invNormal = VEC3F_Y;
		};
	} // namespace Renderer
} // namespace VTX

#endif
