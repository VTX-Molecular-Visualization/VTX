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
				// compute direct lighting from all lights in the scene
				for ( const BaseLight * light : p_scene.getLights() )
				{
					const Vec3f lightDir		   = light->sample( intersection._point );
					const float lightDistSqr	   = Util::Math::dot( lightDir, lightDir );
					const float lightDist		   = sqrtf( lightDistSqr );
					const Vec3f lightDirNormalized = lightDir / lightDist;

					const Ray shadowRay( intersection._point, lightDirNormalized );

					if ( !p_scene.intersectAny( shadowRay, 1e-3f, lightDist ) )
					{
						Li += intersection._primitive->getMaterial()->shade( p_ray, intersection, lightDirNormalized )
							  * light->getColor();
					}
				}
			}
			else
			{
				Li = _backgroundColor;
			}
			return Li;
		}
	} // namespace Renderer
} // namespace VTX
