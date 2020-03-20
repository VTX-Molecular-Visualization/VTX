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
			class CameraRayTracing;

		  public:
			RayTracer()	 = default;
			~RayTracer() = default;

			virtual void init( const uint, const uint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint ) override;

		  private:
			void _renderTiles( std::vector<uchar> &		p_image,
							   const CameraRayTracing & p_camera,
							   const uint				p_nbPixelSamples,
							   const uint				p_threadId,
							   const uint				p_nbTilesX,
							   const uint				p_nbTilesY,
							   const uint				p_nbTiles,
							   std::atomic<uint> &		p_nextTile );

			void _renderTile( std::vector<uchar> &	   p_image,
							  const CameraRayTracing & p_camera,
							  const uint			   p_nbPixelSamples,
							  const uint			   p_taskIndex,
							  const uint			   p_nbTilesX,
							  const uint			   p_nbTilesY );

			Vec3f _renderPixel( const CameraRayTracing & p_camera,
								const float				 p_x,
								const float				 p_y,
								const uint				 p_nbPixelSamples );

		  private:
			static const uint TILE_SIZE;

			Vec3f _backgroundColor = VEC3F_XYZ * 0.3f;

			BVH _bvh;
		};
	} // namespace Renderer
} // namespace VTX

#endif
