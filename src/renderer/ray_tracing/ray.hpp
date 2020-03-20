#ifndef __VTX_RENDERER_RAY_TRACER_RAY__
#define __VTX_RENDERER_RAY_TRACER_RAY__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		class Ray
		{
		  public:
			Ray() = default;
			// direction should be normalized
			Ray( const Vec3f & p_origin, const Vec3f & p_direction ) : _origin( p_origin ), _direction( p_direction ) {}
			~Ray()					 = default;
			Ray( const Ray & p_ray ) = default;				// copy constructor
			Ray( Ray && p_ray )		 = default;				// move constructor
			Ray & operator=( const Ray & p_ray ) = default; // = copy
			Ray & operator=( Ray && p_ray ) = default;		// = move

			const Vec3f & getOrigin() const { return _origin; }
			const Vec3f & getDirection() const { return _direction; }

			Vec3f getPointAtT( const float p_t ) const { return _origin + _direction * p_t; }

		  private:
			Vec3f _origin	 = VEC3F_ZERO;
			Vec3f _direction = VEC3F_Z;
		};
	} // namespace Renderer
} // namespace VTX

#endif
