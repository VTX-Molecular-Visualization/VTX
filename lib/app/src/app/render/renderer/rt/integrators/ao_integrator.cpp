#include "app/render/renderer/rt/integrators/ao_integrator.hpp"
#include "app/old_app/util/sampler.hpp"
#include <util/math.hpp>

namespace VTX::App::Render::Renderer::RT::Integrator
{
	Util::Color::Rgba AOIntegrator::Li( const Ray &	  p_ray,
										const Scene & p_scene,
										const float	  p_tMin,
										const float	  p_tMax ) const
	{
		Intersection	  intersection;
		Util::Color::Rgba Li = Util::Color::Rgba::BLACK;

		if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
		{
			// create orthonormal basis around around hit normal
			Mat3f TBN = Util::Math::createOrthonormalBasis( intersection._normal );

			float ao = 0.f;
			for ( uint i = 0; i < _nbSamples; ++i )
			{
				float u = Util::Math::randomFloat(); // cos theta
				float v = Util::Math::randomFloat();

				Vec3f sampleDir = Util::Sampler::cosineWeightedHemisphere( u, v );
				float samplePdf = Util::Sampler::cosineWeightedHemispherePdf( u );

				// transform in local coordinates systems
				Vec3f aoDir = Util::Math::normalize( TBN * sampleDir );
				Ray	  aoRay( intersection._point, aoDir );
				aoRay.offset( intersection._normal );

				if ( !p_scene.intersectAny( aoRay, SHADOW_EPS, _radius - SHADOW_EPS ) )
					ao += u / samplePdf;
			}
			Li = intersection._primitive->getMaterial()->getColor() * ( ao / _nbSamples ) * INV_PIf;
		}
		else
		{
			Li = _backgroundColor;
		}
		return Li;
	} // namespace Renderer
} // namespace VTX::App::Render::Renderer::RT::Integrator
