#include "direct_lighting_integrator.hpp"
#include "util/math.hpp"
#include "util/sampler.hpp"

namespace VTX
{
	namespace Renderer
	{
		Vec3f DirectLightingIntegrator::Li( const Ray &	  p_ray,
											const Scene & p_scene,
											const float	  p_tMin,
											const float	  p_tMax ) const
		{
			// TODO: for now only the same as RaycastIntegrator
			Intersection intersection;
			Vec3f		 Li = VEC3F_ZERO;

			if ( p_scene._bvh.intersect( p_ray, p_tMin, p_tMax, intersection ) )
			{
				// shade primitive
				// point light on camera
				Li = intersection._primitive->getMaterial()->shade( p_ray, intersection, -p_ray.getDirection() );
			}
			else
			{
				Li = _backgroundColor;
			}
			return Li;
		}
	} // namespace Renderer
} // namespace VTX
