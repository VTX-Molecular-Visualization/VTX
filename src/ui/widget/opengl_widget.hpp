#ifndef __VTX_UI_WIDGET_OPENGL__
#define __VTX_UI_WIDGET_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include "renderer/base_renderer.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLWidget>
#ifdef OPTIX_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif

namespace VTX
{
	namespace Renderer
	{
		class GL;
		namespace GLSL
		{
			class ProgramManager;
		}
		class RayTracer;
#ifdef OPTIX_DEFINED
		namespace Optix
		{
			class OptixRayTracer;
		}
#endif
	} // namespace Renderer

	namespace UI
	{
		namespace Widget
		{
			class OpenGLWidget : public QOpenGLWidget
			{
			  public:
				OpenGLWidget( QWidget * p_parent = 0 );
				~OpenGLWidget();

				inline Renderer::BaseRenderer &				  getRenderer() { return *_renderer; }
				inline const Renderer::BaseRenderer &		  getRenderer() const { return *_renderer; }
				inline Renderer::GL &						  getRendererGL() { return *_rendererGL; }
				inline const Renderer::GL &					  getRendererGL() const { return *_rendererGL; }
				inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _renderer->getProgramManager(); }
				inline const Renderer::GLSL::ProgramManager & getProgramManager() const { return _renderer->getProgramManager(); }
				inline const QOpenGLFunctions_4_5_Core &	  getFunctions() const { return *_functions; }
				inline QOpenGLFunctions_4_5_Core &			  getFunctions() { return *_functions; }

				void switchRenderer( const Renderer::MODE );

				void initializeGL() override;
				void paintGL() override;
				void resizeGL( int, int ) override;

			  private:
				QOpenGLFunctions_4_5_Core * _functions	= nullptr;
				Renderer::BaseRenderer *	_renderer	= nullptr;
				Renderer::GL *				_rendererGL = nullptr;
#ifdef CUDA_DEFINED
				Renderer::RayTracer * _rendererRT = nullptr;
#endif
#ifdef OPTIX_DEFINED
				Renderer::Optix::OptixRayTracer * _rendererOptix = nullptr;
#endif
			};
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX

#endif
