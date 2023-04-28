#ifndef __VTX_RENDERER_RAY_TRACER_FLAT_COLOR_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_FLAT_COLOR_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_material.hpp"

namespace VTX
{
	namespace Renderer
	{
		class FlatColorMaterial : public BaseMaterial
		{
		  public:
			FlatColorMaterial( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

			Util::Color::Rgba getColor() const override { return _color; }

			Util::Color::Rgba shade( const Ray &			p_ray,
							   const Intersection & p_hit,
							   const LightSample &	p_lightSample ) const override
			{
				return _color;
			}

		  private:
			Util::Color::Rgba _color;
		};
	} // namespace Renderer
} // namespace VTX

#endif
