#ifndef __VTX_RENDERER_RAY_TRACER_SCENE__
#define __VTX_RENDERER_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "lights/base_light.hpp"
#include "primitives/base_object_3D.hpp"
#include "primitives/molecule_rt.hpp"

namespace VTX
{
	namespace Renderer
	{
		class Scene
		{
		  public:
			Scene() = default;
			~Scene()
			{
				for ( BaseLight * light : _lights )
				{
					delete light;
				}

				for ( BaseObject3D * object : _objects )
				{
					delete object;
				}
			}

			const BaseObject3D *			 getObject( const uint p_id ) const { return _objects[ p_id ]; }
			const std::vector<BaseLight *> & getLights() const { return _lights; }

			void clean()
			{
				for ( BaseLight * light : _lights )
				{
					delete light;
				}

				for ( BaseObject3D * object : _objects )
				{
					delete object;
				}

				_aabb.invalidate();
			}

			// returns nearest intersection if exists
			bool intersect( const Ray &	   p_ray,
							const float	   p_tMin,
							const float	   p_tMax,
							Intersection & p_intersection ) const
			{
				float tMax = p_tMax;
				bool  hit  = false;

				for ( BaseObject3D * object : _objects )
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
				for ( BaseObject3D * object : _objects )
				{
					if ( object->intersectAny( p_ray, p_tMin, p_tMax ) ) { return true; }
				}
				return false;
			}

			void addObject( BaseObject3D * p_object )
			{
				_objects.emplace_back( p_object );
				_aabb.extend( p_object->getAABB() );
			}

			void addLight( BaseLight * p_light ) { _lights.emplace_back( p_light ); }

		  private:
			std::vector<BaseObject3D *> _objects;
			std::vector<BaseLight *>	_lights;
			Math::AABB					_aabb;
		};
	} // namespace Renderer
} // namespace VTX

#endif
