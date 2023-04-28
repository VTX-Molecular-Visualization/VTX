#ifndef __VTX_RENDERER_RAY_TRACER_BRDF_OREN_NAYAR__
#define __VTX_RENDERER_RAY_TRACER_BRDF_OREN_NAYAR__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include <util/math.hpp>

namespace VTX
{
	namespace Renderer
	{
		namespace BRDF
		{
			// See:
			// Oren M., Nayar Shree K. (1994)
			// Generalization of Lambert's Reflectance Model
			// Proceedings of the 21st Annual Conference on Computer Graphics and Interactive Techniques
			namespace OrenNayar
			{
				// TODO: optimize it ! acos/sin/tan can be removed...
				inline Util::Color::Rgba fr( const Intersection & p_hit,
									   const Vec3f &		p_wo,
									   const Vec3f &		p_wi,
									   const Util::Color::Rgba &	p_albedo,
									   const float			p_roughness )
				{
					const float roughness2 = p_roughness * p_roughness;
					// 0.33 is replaced by 0.57 to better approximate surface interreflection
					const float A = 1.f - 0.5f * ( roughness2 / ( roughness2 + 0.57f ) );
					const float B = 0.45f * ( roughness2 / ( roughness2 + 0.09f ) );

					const float cosThetaO = Util::Math::clamp( Util::Math::dot( p_hit._normal, p_wo ), 0.f, 1.f );
					const float cosThetaI = Util::Math::clamp( Util::Math::dot( p_hit._normal, p_wi ), 0.f, 1.f );

					// projections in the plane orthogonal to the normal
					const Vec3f woTB = Util::Math::normalize( p_wo - p_hit._normal * cosThetaO );
					const Vec3f wiTB = Util::Math::normalize( p_wi - p_hit._normal * cosThetaI );

					const float cosPhi = Util::Math::dot( woTB, wiTB );
					const float gamma  = Util::Math::max( 0.f, cosPhi );

					const float thetaO = acosf( cosThetaO );
					const float thetaI = acosf( cosThetaI );
					const float alpha  = Util::Math::max( thetaO, thetaI );
					const float beta   = Util::Math::min( thetaO, thetaI );
					const float C	   = sinf( alpha ) * tanf( beta );
					return p_albedo * INV_PIf * ( A + B * gamma * C );
				}
			} // namespace OrenNayar

		} // namespace BRDF

	} // namespace Renderer
} // namespace VTX

#endif
