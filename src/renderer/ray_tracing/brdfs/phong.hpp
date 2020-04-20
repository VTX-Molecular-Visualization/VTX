#ifndef __VTX_RENDERER_RAY_TRACER_BRDF_PHONG__
#define __VTX_RENDERER_RAY_TRACER_BRDF_PHONG__

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
			namespace Phong
			{
				inline Vec3f fr( const Intersection & p_hit,
								 const Vec3f &		  p_wo,
								 const Vec3f &		  p_wi,
								 const Vec3f &		  p_albedo,
								 const float &		  p_shininess )
				{
					const Vec3f reflect = Util::Math::reflect( -p_wi, p_hit._normal );
					return p_albedo
						   * Util::Math::pow( Util::Math::max( 0.f, Util::Math::dot( reflect, p_wo ) ), p_shininess );
				}
			} // namespace Phong

		} // namespace BRDF

	} // namespace Renderer
} // namespace VTX

#endif
