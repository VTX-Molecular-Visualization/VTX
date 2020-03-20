#ifndef __VTX_RENDERER_RAY_TRACER__
#define __VTX_RENDERER_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_renderer.hpp"
#include "ray_tracing/bvh.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		class RayTracer : public BaseRenderer
		{
		  public:
			RayTracer()	 = default;
			~RayTracer() = default;

			virtual void init( const uint, const uint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint ) override;

		  private:
			void _renderTile( std::vector<uchar> &	   p_image,
							  const Object3D::Camera & p_camera,
							  const uint			   p_nbPixelSamples,
							  const uint			   p_taskIndex,
							  const uint			   p_threadIndex,
							  const uint			   p_numTilesX,
							  const uint			   p_numTilesY );

			Vec3f _renderPixel( const Object3D::Camera & p_camera,
								const float				 p_x,
								const float				 p_y,
								const uint				 p_nbPixelSamples );

		  private:
			const uint TILE_SIZE = 16;

			Vec3f _backgroundColor = VEC3F_ZERO;

			BVH _bvh;
		};
	} // namespace Renderer
} // namespace VTX

#endif
