#ifndef __VTX_UI_WIDGET_OPENGL__
#define __VTX_UI_WIDGET_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_opengl.hpp"
#include "renderer/base_renderer.hpp"
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QPainter>

namespace VTX
{
	namespace Renderer
	{
		namespace GL
		{
			class GL;
		} // namespace GL
		class RayTracer;
#ifdef OPTIX_DEFINED
		namespace Optix
		{
			class OptixRayTracer;
		}
#endif
	} // namespace Renderer

	namespace UI::Widget::Render
	{
		class OpenGLWidget : public QOpenGLWidget, public Generic::BaseOpenGL
		{
		  public:
			OpenGLWidget( QWidget * p_parent = 0 );
			~OpenGLWidget();

			inline Renderer::BaseRenderer &		  getRenderer() { return *_renderer; }
			inline const Renderer::BaseRenderer & getRenderer() const { return *_renderer; }
			inline Renderer::GL::GL &			  getRendererGL() { return *_rendererGL; }
			inline const Renderer::GL::GL &		  getRendererGL() const { return *_rendererGL; }

			void initializeGL() override;
			void paintGL() override;
			void resizeGL( int, int ) override;

			void switchRenderer( const Renderer::MODE );

		  private:
			QElapsedTimer _timer		= QElapsedTimer();
			QElapsedTimer _frameTimer	= QElapsedTimer();
			uint		  _frameCounter = 0u;
			QPainter	  _painter		= QPainter();

			Renderer::BaseRenderer * _renderer	 = nullptr;
			Renderer::GL::GL *		 _rendererGL = nullptr;
#ifdef CUDA_DEFINED
			Renderer::RayTracer * _rendererRT = nullptr;
#endif
#ifdef OPTIX_DEFINED
			Renderer::Optix::OptixRayTracer * _rendererOptix = nullptr;
#endif
		};
	} // namespace UI::Widget::Render

} // namespace VTX

#endif
