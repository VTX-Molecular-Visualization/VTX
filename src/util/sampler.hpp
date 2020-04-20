#ifndef __VTX_SAMPLER__
#define __VTX_SAMPLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Sampler
		{
			inline Vec3f uniformHemisphere( const float p_u, const float p_v )
			{
				const float cosTheta = p_u;
				const float phi		 = 2.f * PIf * p_v;
				const float sinTheta = sqrtf( Util::Math::max( 1.f - cosTheta * cosTheta, 0.f ) );

				return Vec3f( cosf( phi ) * sinTheta, sinf( phi ) * sinTheta, cosTheta );
			}
			static inline float uniformHemispherePdf() { return INV_2PIf; }
		} // namespace Sampler
	}	  // namespace Util
} // namespace VTX

#endif
