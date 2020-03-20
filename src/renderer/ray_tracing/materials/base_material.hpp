#ifndef __VTX_RENDERER_RAY_TRACER_BASE_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_BASE_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "../ray.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseMaterial
		{
		  public:
			virtual ~BaseMaterial() = default;

			virtual Vec3f shade( const Ray & p_ray, const Intersection & p_hit, const Vec3f & p_lightDir ) const = 0;

		  protected:
		};
	} // namespace Renderer
} // namespace VTX

#endif
