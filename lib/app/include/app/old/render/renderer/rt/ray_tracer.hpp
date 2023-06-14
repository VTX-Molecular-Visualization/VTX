#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_RAY_TRACER__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_RAY_TRACER__

#include "app/old/render/renderer/base_renderer.hpp"
#include "app/old/render/renderer/gl/framebuffer.hpp"
#include "app/old/render/renderer/gl/texture_2d.hpp"
#include "console_progress_bar.hpp"
#include "integrators/base_integrator.hpp"
#include "rt_scene.hpp"
#include <vector>

namespace VTX::App::Old::Render::Renderer::RT
{
	class RayTracer : public App::Old::Render::Renderer::BaseRenderer
	{
		class CameraRayTracing;

	  public:
		RayTracer()	 = default;
		~RayTracer() = default;

		void init( const uint, const uint, const GLuint p_outputFramebufferId ) override;
		void renderFrame( const App::Old::Application::Scene & ) override;
		void updateRenderSetting( const App::Old::Render::Renderer::RENDER_SETTING ) override {}

		void resize( const uint, const uint, const GLuint ) override;

		const inline std::vector<float> & getPixels() const { return _pixels; }

	  private:
		void _initScene( const App::Old::Application::Scene & );

		void _renderTiles( std::vector<float> &		p_image,
						   const CameraRayTracing & p_camera,
						   const uint				p_nbPixelSamples,
						   const uint				p_threadId,
						   const uint				p_nbTilesX,
						   const uint				p_nbTilesY,
						   const uint				p_nbTiles,
						   std::atomic<uint> &		p_nextTileId );

		Util::Color::Rgba _renderPixel( const CameraRayTracing & p_camera,
										const float				 p_x,
										const float				 p_y,
										const uint				 p_nbPixelSamples );

	  private:
		static const uint			 TILE_SIZE;
		Integrator::BaseIntegrator * _integrator   = nullptr;
		Integrator::BaseIntegrator * _aoIntegrator = nullptr;

		Scene			   _scene;
		std::vector<float> _pixels = std::vector<float>();

		App::Old::Render::Renderer::GL::Texture2D _texture = App::Old::Render::Renderer::GL::Texture2D();

		// TODO: keep gamma as float ?
		float _gamma = 1.f;

		ConsoleProgressBar _progressBar;
	};
} // namespace VTX::App::Old::Render::Renderer::RT

#endif
