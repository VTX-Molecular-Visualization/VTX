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
			BaseLight( const Color::Rgba & p_color, const float p_power = 1.f ) : _color( p_color ), _power( p_power )
			{
			}
			virtual ~BaseLight() = default;

			const Color::Rgba & getColor() const { return _color; }
			const float			getPower() const { return _power; }

			virtual LightSample sample( const Vec3f & p_point ) const = 0;

			const bool isSurface() const { return _isSurface; }

		  protected:
			Color::Rgba _color = Color::Rgba::WHITE;
			// TODO: keep it ?
			float _power = 1.f;

			float _pdf = 1.f;

			bool _isSurface = false;
		};
	} // namespace Renderer
} // namespace VTX

#endif
