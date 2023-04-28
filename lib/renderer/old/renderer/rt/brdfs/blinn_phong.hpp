#ifndef __VTX_RENDERER_RAY_TRACER_BRDF_BLINN_PHONG__
#define __VTX_RENDERER_RAY_TRACER_BRDF_BLINN_PHONG__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace BRDF
		{
			namespace BlinnPhong
			{
				inline Util::Color::Rgba fr( const Intersection & p_hit,
									   const Vec3f &		p_wo,
									   const Vec3f &		p_wi,
									   const Util::Color::Rgba &	p_albedo,
									   const float &		p_shininess )
				{
					const Vec3f h = Util::Math::normalize( p_wo + p_wi );

					return p_albedo
						   * Util::Math::pow( Util::Math::max( 0.f, Util::Math::dot( h, p_hit._normal ) ),
											  p_shininess );
				}
			} // namespace BlinnPhong

		} // namespace BRDF

	} // namespace Renderer
} // namespace VTX

#endif
