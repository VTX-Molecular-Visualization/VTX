#ifndef __VTX_APP_INTERNAL_MATH_SAMPLER__
#define __VTX_APP_INTERNAL_MATH_SAMPLER__

#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::App::Internal::Math::Sampler
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

} // namespace VTX::App::Internal::Math::Sampler

#endif
