#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER__
#define __VTX_RENDERER_OPTIX_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../base_renderer.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		class OptixRayTracer : public BaseRenderer
		{
		  public:
			OptixRayTracer()  = default;
			~OptixRayTracer() = default;

			virtual void init( const uint, const uint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint ) override;

		  private:
		};
	} // namespace Renderer
} // namespace VTX

#endif
