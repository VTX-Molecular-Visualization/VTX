#ifndef __VTX_RENDERER_RAY_TRACER__
#define __VTX_RENDERER_RAY_TRACER__

#ifdef _MSC_VER
#pragma once
#endif

#include "console_progress_bar.hpp"
#include "integrators/base_integrator.hpp"
#include "renderer/base_renderer.hpp"
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
			RayTracer( OpenGLFunctions * const p_gl ) : BaseRenderer( p_gl ) {}
			~RayTracer() = default;

			virtual void init( const uint, const uint, const GLuint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint, const GLuint ) override;

			const inline std::vector<uchar> & getPixels() const { return _pixels; }

		  private:
			void _initScene( const Object3D::Scene & );

			void _renderTiles( std::vector<uchar> &		p_image,
							   const CameraRayTracing & p_camera,
							   const uint				p_nbPixelSamples,
							   const uint				p_threadId,
							   const uint				p_nbTilesX,
							   const uint				p_nbTilesY,
							   const uint				p_nbTiles,
							   std::atomic<uint> &		p_nextTileId );

			Color::Rgb _renderPixel( const CameraRayTracing & p_camera,
									 const float			  p_x,
									 const float			  p_y,
									 const uint				  p_nbPixelSamples );

		  private:
			static const uint TILE_SIZE;
			BaseIntegrator *  _integrator	= nullptr;
			BaseIntegrator *  _aoIntegrator = nullptr;

			Scene			   _scene;
			std::vector<uchar> _pixels	= std::vector<uchar>();
			GLuint			   _texture = GL_INVALID_VALUE;

			// TODO: keep gamma as float ?
			float _gamma = 1.f;

			ConsoleProgressBar _progressBar;
		};
	} // namespace Renderer
} // namespace VTX

#endif
