#ifndef __VTX_RENDERER_RAY_TRACER__
#define __VTX_RENDERER_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "console_progress_bar.hpp"
#include "integrators/base_integrator.hpp"
#include "old_app/renderer/base_renderer.hpp"
#include "old_app/renderer/gl/framebuffer.hpp"
#include "old_app/renderer/gl/texture_2d.hpp"
#include "rt_scene.hpp"
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

			void init( const uint, const uint, const GLuint p_outputFramebufferId ) override;
			void renderFrame( const Object3D::Scene & ) override;
			void updateRenderSetting( const RENDER_SETTING ) override {}

			void resize( const uint, const uint, const GLuint ) override;

			const inline std::vector<float> & getPixels() const { return _pixels; }

		  private:
			void _initScene( const Object3D::Scene & );

			void _renderTiles( std::vector<float> &		p_image,
							   const CameraRayTracing & p_camera,
							   const uint				p_nbPixelSamples,
							   const uint				p_threadId,
							   const uint				p_nbTilesX,
							   const uint				p_nbTilesY,
							   const uint				p_nbTiles,
							   std::atomic<uint> &		p_nextTileId );

			Color::Rgba _renderPixel( const CameraRayTracing & p_camera,
									  const float			   p_x,
									  const float			   p_y,
									  const uint			   p_nbPixelSamples );

		  private:
			static const uint TILE_SIZE;
			BaseIntegrator *  _integrator	= nullptr;
			BaseIntegrator *  _aoIntegrator = nullptr;

			Scene			   _scene;
			std::vector<float> _pixels = std::vector<float>();

			GL::Texture2D _texture = GL::Texture2D();

			// TODO: keep gamma as float ?
			float _gamma = 1.f;

			ConsoleProgressBar _progressBar;
		};
	} // namespace Renderer
} // namespace VTX

#endif
