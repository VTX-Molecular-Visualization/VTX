#ifndef __VTX_UTIL_SAMPLER__
#define __VTX_UTIL_SAMPLER__

#include <util/types.hpp>
#include <util/math.hpp>

namespace VTX
{
	namespace Util
	{
		namespace Sampler
		{
			// Uniform hemisphere sampling: z is up
			inline Vec3f uniformHemisphere( const float p_u, const float p_v )
			{
				const float cosTheta = p_u;
				const float phi		 = TWO_PIf * p_v;
				const float sinTheta = sqrtf( 1.f - cosTheta * cosTheta );
				return Vec3f( cosf( phi ) * sinTheta, sinf( phi ) * sinTheta, cosTheta );
			}

			inline float uniformHemispherePdf() { return INV_2PIf; }

			// Cosine weighted hemisphere samping: z is up
			inline Vec3f cosineWeightedHemisphere( const float p_u, const float p_v )
			{
				const float cosTheta = sqrtf( p_u );
				const float sinTheta = sqrtf( 1.f - p_u );
				const float phi		 = TWO_PIf * p_v;
				return Vec3f( cosf( phi ) * sinTheta, sinf( phi ) * sinTheta, cosTheta );
			}

			inline float cosineWeightedHemispherePdf( const float p_cosTheta ) { return p_cosTheta * INV_PIf; }

		} // namespace Sampler
	}	  // namespace Util
} // namespace VTX

#endif
