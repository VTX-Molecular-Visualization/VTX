#ifndef __VTX_RENDERER_RAY_TRACER_LIGHT_SAMPLE__
#define __VTX_RENDERER_RAY_TRACER_LIGHT_SAMPLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		struct LightSample
		{
			LightSample( const Vec3f &		p_dir,
						 const float		p_distance,
						 const Color::Rgb & p_radiance,
						 const float		p_pdf ) :
				_dir( p_dir ),
				_distance( p_distance ), _radiance( p_radiance ), _pdf( p_pdf )
			{
			}

			Vec3f	   _dir		 = VEC3F_ZERO;		  // normalized direction toward light source
			float	   _distance = 0.f;				  // dir length
			Color::Rgb _radiance = Color::Rgb::BLACK; // radiance at given point divided by pdf
			float	   _pdf		 = 1.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
