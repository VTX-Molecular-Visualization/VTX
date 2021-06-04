#ifndef __VTX_RENDERER_RAY_TRACER_INTERSECTION__
#define __VTX_RENDERER_RAY_TRACER_INTERSECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BasePrimitive;

		struct Intersection
		{
			Intersection() = default;
			Intersection( const Vec3f &				  p_point,
						  const Vec3f &				  p_normal,
						  const float				  p_distance,
						  const BasePrimitive * const p_prim ) :
				_point( p_point ),
				_normal( p_normal ), _distance( p_distance ), _primitive( p_prim )
			{
			}
			Intersection( const Intersection & p_intersection ) = default;			   // copy constructor
			Intersection( Intersection && p_intersection )		= default;			   // move constructor
			Intersection & operator=( const Intersection & p_intersection ) = default; // = copy
			Intersection & operator=( Intersection && p_intersection ) = default;	   // = move

			Vec3f				  _point	 = VEC3F_ZERO;
			Vec3f				  _normal	 = VEC3F_ZERO;
			float				  _distance	 = -1.f;
			const BasePrimitive * _primitive = nullptr;
		};
		static const Intersection NO_INTERSECTION = Intersection();
	} // namespace Renderer
} // namespace VTX

#endif
