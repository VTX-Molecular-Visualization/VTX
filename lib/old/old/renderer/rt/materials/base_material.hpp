#ifndef __VTX_RENDERER_RAY_TRACER_BASE_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_BASE_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "../lights/light_sample.hpp"
#include "../ray.hpp"
#include "color/rgb.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseMaterial
		{
		  public:
			BaseMaterial()			= default;
			virtual ~BaseMaterial() = default;

			virtual Color::Rgb getColor() const = 0;

			virtual Color::Rgb shade( const Ray &		   p_ray,
									  const Intersection & p_hit,
									  const LightSample &  p_lightDirSample ) const
				= 0;
		};
	} // namespace Renderer
} // namespace VTX

#endif
