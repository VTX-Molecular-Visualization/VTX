#ifndef __VTX_APP_RENDER_RENDERER_RT_BASE_PRIMITIVE__
#define __VTX_APP_RENDER_RENDERER_RT_BASE_PRIMITIVE__

#include "../intersection.hpp"
#include "../materials/base_material.hpp"
#include "base_object_3D.hpp"

namespace VTX::App::Render::Renderer::RT::Primitive
{
	class BasePrimitive : public BaseObject3D
	{
	  public:
		BasePrimitive() = default;
		BasePrimitive( Material::BaseMaterial * const p_mtl ) : _material( p_mtl ) {}
		virtual ~BasePrimitive() = default;

		const Material::BaseMaterial *				   getMaterial() const { return _material; }
		const App::Component::Object3D::Helper::AABB & getAABB() const { return _aabb; }

		virtual bool intersect( const Ray &	   p_ray,
								const float	   p_tMin,
								const float	   p_tMax,
								Intersection & p_intersection ) const
			= 0;

		virtual bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
		{
			// TODO: add a method to avoid this Intersection()
			Intersection inter = Intersection( VEC3F_ZERO, VEC3F_ZERO, FLOAT_MAX, nullptr );
			return intersect( p_ray, p_tMin, p_tMax, inter );
		}

	  protected:
		Material::BaseMaterial * _material = nullptr;

	  private:
		virtual void _computeAABB() = 0;
	};
} // namespace VTX::App::Render::Renderer::RT::Primitive

#endif
