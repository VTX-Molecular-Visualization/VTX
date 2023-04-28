#ifndef __VTX_RENDERER_RAY_TRACER_BRDF_LAMBERT__
#define __VTX_RENDERER_RAY_TRACER_BRDF_LAMBERT__

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
			namespace Lambert
			{
				inline Util::Color::Rgba fr( const Intersection & p_hit,
									   const Vec3f &		p_wo,
									   const Vec3f &		p_wi,
									   const Util::Color::Rgba &	p_albedo )
				{
					return p_albedo * INV_PIf;
				}
			} // namespace Lambert

		} // namespace BRDF

	} // namespace Renderer
} // namespace VTX

#endif
