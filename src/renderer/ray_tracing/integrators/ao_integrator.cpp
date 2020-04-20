#include "ao_integrator.hpp"
#include "util/math.hpp"
#include "util/sampler.hpp"

namespace VTX
{
	namespace Renderer
	{
		Vec3f AOIntegrator::Li( const Ray & p_ray, const Scene & p_scene, const float p_tMin, const float p_tMax ) const
		{
			Intersection intersection;
			Vec3f		 Li = VEC3F_ZERO;

			if ( p_scene.intersect( p_ray, p_tMin, p_tMax, intersection ) )
			{
				// create orthonormal basis around around hit normal
				Mat3f TBN = Util::Math::createOrthonormalBasis( intersection._normal );

				const uint	aoSamples = 32;
				const float aoRadius  = 20.f;
				float		ao		  = 0.f;
				for ( uint i = 0; i < aoSamples; ++i )
				{
					float u		 = Util::Math::randomFloat();
					float v		 = Util::Math::randomFloat();
					Vec3f sample = Util::Sampler::uniformHemisphere( u, v );
					float pdf	 = Util::Sampler::uniformHemispherePdf();
					// transform in local coordinates systems
					Vec3f aoDir = TBN * sample;
					Ray	  aoRay( intersection._point, aoDir );
					aoRay.offset( intersection._normal );
					if ( !p_scene.intersectAny( aoRay, 1e-3f, aoRadius ) )
					{
						// u is cos(theta) <=> dot(n, aoDir)
						ao += u / pdf;
					}
				}
				ao /= aoSamples;
				// shade primitive
				// point light on camera
				Li += ao;
			}
			else
			{
				Li += _backgroundColor;
			}
			return Li;
		}
	} // namespace Renderer
} // namespace VTX
