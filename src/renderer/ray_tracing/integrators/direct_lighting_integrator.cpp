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

			if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
			{
				const BaseMaterial * mtl = intersection._primitive->getMaterial();

				// compute direct lighting from all lights in the scene
				for ( const BaseLight * light : p_scene.getLights() )
				{
					const LightSample ls = light->sample( intersection._point );

					const Ray shadowRay( intersection._point, ls._dir );

					if ( !p_scene.intersectAny( shadowRay, 1e-3f, FLT_MAX ) )
					{ Li += 0.9f * mtl->shade( p_ray, intersection, ls._dir ) * ls._radiance; }
				}
				Li += 0.1f * mtl->getColor();
			}
			else
			{
				Li = _backgroundColor;
			}
			return Li;
		}
	} // namespace Renderer
} // namespace VTX
