#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_PLANE__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_PLANE__

#include "base_primitive.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Render::Renderer::RT::Primitive
{
	class Plane : public BasePrimitive
	{
	  public:
		Plane( const Vec3f & p_n, const float p_d, Material::BaseMaterial * const p_mtl ) :
			BasePrimitive( p_mtl ), _n( Util::Math::normalize( p_n ) ), _d( p_d )
		{
		}
		Plane( const Vec3f & p_pt, const Vec3f & p_n, Material::BaseMaterial * const p_mtl ) :
			BasePrimitive( p_mtl ), _n( Util::Math::normalize( p_n ) ), _d( Util::Math::dot( p_pt, _n ) )
		{
		}

		// Inherited via BasePrimitive
		bool intersect( const Ray &	   p_ray,
						const float	   p_tMin,
						const float	   p_tMax,
						Intersection & p_intersection ) const override
		{
			// const Vec3f & o	  = p_ray.getOrigin();
			const Vec3f & d	  = p_ray.getDirection();
			const float	  den = Util::Math::dot( d, _n );
			if ( den == 0.f )
				return false;
			const float t = ( Util::Math::dot( p_ray.getOrigin(), _n ) + _d ) / den;
			if ( t < p_tMin || t > p_tMax )
			{
				return false;
			}

			p_intersection._point	  = p_ray.getPointAtT( t );
			p_intersection._normal	  = Util::Math::faceForward( _n, d );
			p_intersection._distance  = t;
			p_intersection._primitive = this;

			return true;
		}

		void _computeAABB() override
		{
			// keep invalid aabb
		}

	  private:
		Vec3f _n;
		float _d;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Primitive
#endif
