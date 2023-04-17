#ifndef __VTX_RENDERER_RAY_TRACER_BRDF_COOK_TORRANCE__
#define __VTX_RENDERER_RAY_TRACER_BRDF_COOK_TORRANCE__

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
			namespace CookTorrance
			{
				// TODO: GGX or other ?
				inline Color::Rgba fr( const Intersection & p_hit,
									   const Vec3f &		p_wo,
									   const Vec3f &		p_wi,
									   const Color::Rgba &	p_f0,
									   const float			p_shininess )
				{
					const Vec3f h = Util::Math::normalize( p_wo + p_wi );

					const float HdotN = Util::Math::dot( h, p_hit._normal );
					const float OdotH = Util::Math::dot( p_wo, h );

					// Microfacets distribution.
					const float D = ( p_shininess + 2.f ) * Util::Math::pow( HdotN, p_shininess ) / TWO_PIf;
					// Geometric factor.
					const float cosThetaO = Util::Math::dot( p_wo, p_hit._normal );
					const float cosThetaI = Util::Math::dot( p_wo, p_hit._normal );
					const float G
						= Util::Math::min( 1.f, 2.f * HdotN * Util::Math::min( cosThetaO / OdotH, cosThetaI / OdotH ) );

					// Fresnel Schlick's approximation.
					// F = f0 + (1 - f0) * (1-IdotH)^5
					const float		  _IdotH  = 1.f - Util::Math::dot( p_wi, h );
					const float		  _IdotH2 = _IdotH * _IdotH;
					const Color::Rgba F		  = p_f0 + ( 1.f - p_f0 ) * _IdotH2 * _IdotH2 * _IdotH;

					return ( F * D * G ) / ( 4.f * cosThetaO * cosThetaI );
				}
			} // namespace CookTorrance

		} // namespace BRDF

	} // namespace Renderer
} // namespace VTX

#endif
