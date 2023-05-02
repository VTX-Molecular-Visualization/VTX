#ifndef __VTX_APP_RENDER_RENDERER_RT_RT_SCENE__
#define __VTX_APP_RENDER_RENDERER_RT_RT_SCENE__

#include "lights/base_light.hpp"
#include "primitives/base_object_3D.hpp"
#include "primitives/molecule_rt.hpp"
#include <util/types.hpp>

namespace VTX::App::Render::Renderer::RT
{
	class Scene
	{
	  public:
		Scene() = default;
		~Scene()
		{
			for ( Light::BaseLight * light : _lights )
			{
				delete light;
			}

			for ( Primitive::BaseObject3D * object : _objects )
			{
				delete object;
			}
		}

		const Primitive::BaseObject3D *			getObject( const uint p_id ) const { return _objects[ p_id ]; }
		const std::vector<Light::BaseLight *> & getLights() const { return _lights; }

		void clean()
		{
			for ( Light::BaseLight * light : _lights )
			{
				delete light;
			}

			for ( Primitive::BaseObject3D * object : _objects )
			{
				delete object;
			}

			_aabb.invalidate();
		}

		// returns nearest intersection if exists
		bool intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, Intersection & p_intersection ) const
		{
			float tMax = p_tMax;
			bool  hit  = false;

			for ( Primitive::BaseObject3D * object : _objects )
			{
				if ( object->intersect( p_ray, p_tMin, tMax, p_intersection ) )
				{
					tMax = p_intersection._distance;
					hit	 = true;
				}
			}

			return hit;
		}

		// return true if any intersection is found
		bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
		{
			for ( Primitive::BaseObject3D * object : _objects )
			{
				if ( object->intersectAny( p_ray, p_tMin, p_tMax ) )
				{
					return true;
				}
			}
			return false;
		}

		void addObject( Primitive::BaseObject3D * p_object )
		{
			_objects.emplace_back( p_object );
			_aabb.extend( p_object->getAABB() );
		}

		void addLight( Light::BaseLight * p_light ) { _lights.emplace_back( p_light ); }

	  private:
		std::vector<Primitive::BaseObject3D *> _objects;
		std::vector<Light::BaseLight *>		   _lights;
		App::Component::Object3D::Helper::AABB _aabb;
	};
} // namespace VTX::App::Render::Renderer::RT

#endif
