#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_CYLINDER__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_CYLINDER__

#include "base_primitive.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Render::Renderer::RT::Primitive
{
	class Cylinder : public BasePrimitive
	{
	  public:
		~Cylinder() = default;
		Cylinder( const Vec3f & p_v0, const Vec3f & p_v1, const float p_radius, Material::BaseMaterial * const p_mtl ) :
			BasePrimitive( p_mtl ), _v0( p_v0 ), _v1( p_v1 ), _radius( p_radius )
		{
			_computeAABB();
		}
		Cylinder( const Cylinder & p_node )			   = default; // copy constructor
		Cylinder( Cylinder && p_ray )				   = default; // move constructor
		Cylinder & operator=( const Cylinder & p_ray ) = default; // = copy
		Cylinder & operator=( Cylinder && p_ray )	   = default; // = move

		const float radius() const { return _radius; }

		bool intersect( const Ray &	   p_ray,
						const float	   p_tMin,
						const float	   p_tMax,
						Intersection & p_intersection ) const override
		{
			const Vec3f & o	  = p_ray.getOrigin();
			const Vec3f & d	  = p_ray.getDirection();
			const Vec3f	  ov0 = o - _v0;
			const Vec3f	  v	  = _v1 - _v0;

			const float d0 = Util::Math::dot( v, v );
			const float d1 = Util::Math::dot( v, d );
			const float d2 = Util::Math::dot( v, ov0 );

			const float a = d0 - d1 * d1;
			const float b = d0 * Util::Math::dot( ov0, d ) - d2 * d1;
			const float c = d0 * Util::Math::dot( ov0, ov0 ) - d2 * d2 - _radius * _radius * d0;

			const float h = b * b - a * c;

			if ( h < 0.f )
			{
				return false;
			}

			float t = ( -b - sqrt( h ) ) / a;
			if ( t < p_tMin || t > p_tMax )
			{
				return false;
			}

			const float y = d2 + t * d1;
			if ( y < 0.f || y > d0 )
			{
				return false;
			}

			p_intersection._point	  = p_ray.getPointAtT( t );
			const Vec3f normal		  = ( ov0 + t * d - v * y / d0 ) / _radius;
			p_intersection._normal	  = Util::Math::faceForward( normal, d );
			p_intersection._distance  = t;
			p_intersection._primitive = this;

			return true;
		}

	  private:
		void _computeAABB() override
		{
			const Vec3f v = _v1 - _v0;
			const Vec3f e = _radius * sqrt( 1.f - v * v / dot( v, v ) );

			_aabb = App::Old::Component::Object3D::Helper::AABB( Util::Math::min( _v0 - e, _v1 - e ),
															Util::Math::max( _v0 + e, _v1 + e ) );
		}

	  private:
		Vec3f _v0	  = VEC3F_ZERO;
		Vec3f _v1	  = VEC3F_ZERO;
		float _radius = 1.f;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Primitive

#endif
