#ifndef __VTX_UTIL_SAMPLER__
#define __VTX_UTIL_SAMPLER__

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

			inline float uniformHemispherePdf() { return INV_2PIf; }

			inline Vec3f cosineWeightedHemisphere( const float p_u, const float p_v )
			{
				const float sqrtCosTheta = sqrtf( p_u );
				const float phi			 = TWO_PIf * p_v;
				const float z			 = sqrtf( 1.f - p_u );

				return Vec3f( sqrtCosTheta * cosf( phi ), sqrtCosTheta * sinf( phi ), z );
			}

			inline float cosineWeightedHemispherePdf( const float cosTheta ) { return cosTheta * INV_PIf; }

		} // namespace Sampler
	}	  // namespace Util
} // namespace VTX

#endif
