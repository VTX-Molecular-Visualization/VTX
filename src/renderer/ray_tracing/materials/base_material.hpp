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
			explicit BaseMaterial( const Vec3f & p_color ) : _color( p_color ) {}
			virtual ~BaseMaterial() = default;

			const Vec3f & getColor() const { return _color; }

			virtual Vec3f shade( const Ray & p_ray, const Intersection & p_hit, const Vec3f & p_lightDir ) const = 0;

		  protected:
			Vec3f _color;
		};
	} // namespace Renderer
} // namespace VTX

#endif
