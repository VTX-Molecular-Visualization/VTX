#ifndef __VTX_RENDERER_RAY_TRACER_BASE_LIGHT__
#define __VTX_RENDERER_RAY_TRACER_BASE_LIGHT__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "light_sample.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseLight
		{
		  public:
			BaseLight( const Vec3f & p_color, const float p_power = 1.f ) : _color( p_color ), _power( p_power ) {}
			virtual ~BaseLight() = default;

			const Vec3f & getColor() const { return _color; }
			const float	  getPower() const { return _power; }

			virtual LightSample sample( const Vec3f & p_point ) const = 0;

			const float getPdf() const { return _pdf; }

		  protected:
			Vec3f _color = VEC3F_XYZ;
			// TODO: keep it ?
			float _power = 1.f;

			float _pdf = 1.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
