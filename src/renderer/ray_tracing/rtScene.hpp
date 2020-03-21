#ifndef __VTX_RENDERER_RAY_TRACER_SCENE__
#define __VTX_RENDERER_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "bvh.hpp"
#include "define.hpp"
#include "lights/base_light.hpp"

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
					delete light;
			}

			const std::vector<BaseLight *> & getLights() const { return _lights; }

			void buildBVH( const std::vector<BasePrimitive *> & p_prims,
						   const uint							p_maxPrimsLeaf,
						   const BVH::SplitMethod				p_splitMethod )
			{
				_bvh.build( p_prims, p_maxPrimsLeaf, p_splitMethod );
			}

			// returns nearest intersection if exists
			bool intersect( const Ray &	   p_ray,
							const float	   p_tMin,
							const float	   p_tMax,
							Intersection & p_intersection ) const
			{
				return _bvh.intersect( p_ray, p_tMin, p_tMax, p_intersection );
			}

			// return true if any intersection is found
			bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
			{
				return _bvh.intersectAny( p_ray, p_tMin, p_tMax );
			}

			void addLight( BaseLight * p_light ) { _lights.emplace_back( p_light ); }

		  private:
			BVH						 _bvh;
			std::vector<BaseLight *> _lights;
		};
	} // namespace Renderer
} // namespace VTX

#endif
