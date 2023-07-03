#include "app/old/render/renderer/rt/integrators/raycast_integrator.hpp"

namespace VTX::App::Old::Render::Renderer::RT::Integrator
{
	Util::Color::Rgba RayCastIntegrator::Li( const Ray &   p_ray,
											 const Scene & p_scene,
											 const float   p_tMin,
											 const float   p_tMax ) const
	{
		Intersection	  intersection;
		Util::Color::Rgba Li = COLOR_BLACK;

		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
		{
			// shade primitive
			// point light on camera
			Light::LightSample ls( -p_ray.getDirection(), 1.f, COLOR_WHITE, 1.f );

			const float cosTheta = Util::Math::dot( intersection._normal, ls._dir );

			Li = intersection._primitive->getMaterial()->shade( p_ray, intersection, ls ) * cosTheta;
		}
		else
		{
			Li = _backgroundColor;
		}
		return Li;
	}

} // namespace VTX::App::Old::Render::Renderer::RT::Integrator
