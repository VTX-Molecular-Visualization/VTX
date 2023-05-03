#include "app/render/renderer/rt/integrators/direct_lighting_integrator.hpp"
#include <util/math.hpp>

namespace VTX::App::Render::Renderer::RT::Integrator
{
	Util::Color::Rgba DirectLightingIntegrator::Li( const Ray &	  p_ray,
													const Scene & p_scene,
													const float	  p_tMin,
													const float	  p_tMax ) const
	{
		// TODO: for now only the same as RaycastIntegrator
		Intersection	  intersection;
		Util::Color::Rgba Li = Util::Color::Rgba::BLACK;

		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
		{
			const Material::BaseMaterial * mtl = intersection._primitive->getMaterial();

			// compute direct lighting from all lights in the scene
			for ( const Light::BaseLight * light : p_scene.getLights() )
			{
				const uint nbLightSamples = light->isSurface() ? 32 : 1;

				Util::Color::Rgba lightContrib = Util::Color::Rgba::BLACK;
				for ( uint i = 0; i < nbLightSamples; ++i )
				{
					const Light::LightSample ls = light->sample( intersection._point );

					Ray shadowRay( intersection._point, ls._dir );
					shadowRay.offset( intersection._normal );

					const float cosTheta = Util::Math::max( 0.f, Util::Math::dot( intersection._normal, ls._dir ) );

					if ( !p_scene.intersectAny( shadowRay, SHADOW_EPS, ls._distance - SHADOW_EPS ) )
					{
						lightContrib += mtl->shade( p_ray, intersection, ls ) * ls._radiance * cosTheta;
					}
				}
				Li += /*0.9f **/ lightContrib / float( nbLightSamples );
			}
			// Li += 0.1f * mtl->getColor();
		}
		else
		{
			Li = _backgroundColor;
		}
		return Li;
	}
} // namespace VTX::App::Render::Renderer::RT::Integrator
