#include "raycast_integrator.hpp"

namespace VTX
{
	namespace Renderer
	{
		Color RayCastIntegrator::Li( const Ray &   p_ray,
									 const Scene & p_scene,
									 const float   p_tMin,
									 const float   p_tMax ) const
		{
			Intersection intersection;
			Color		 Li = Color::black;

			if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
			{
				// shade primitive
				// point light on camera
				LightSample ls( -p_ray.getDirection(), 1.f, Color::white, 1.f );

				const float cosTheta = Util::Math::dot( intersection._normal, ls._dir );

				Li = intersection._primitive->getMaterial()->shade( p_ray, intersection, ls ) * cosTheta;
			}
			else
			{
				Li = _backgroundColor;
			}
			return Li;
		}

	} // namespace Renderer
} // namespace VTX
