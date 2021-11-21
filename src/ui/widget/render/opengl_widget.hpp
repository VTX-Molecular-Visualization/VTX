#ifndef __VTX_UI_WIDGET_OPENGL__
#define __VTX_UI_WIDGET_OPENGL__

#include "event/base_event_firerer_input.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/base_renderer.hpp"
#include "ui/widget/base_manual_widget.hpp"
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
	} // namespace Renderer

	namespace UI::Widget::Render
	{
		class OpenGLWidget :
			public BaseManualWidget<QOpenGLWidget>,
			public Generic::BaseOpenGL,
			public Event::BaseEventFirererInput
		{
			VTX_WIDGET

		  public:
			~OpenGLWidget();

			void localize() override {}

			inline Renderer::BaseRenderer &		  getRenderer() { return *_renderer; }
			inline const Renderer::BaseRenderer & getRenderer() const { return *_renderer; }
			inline Renderer::GL::GL &			  getRendererGL() { return *_rendererGL; }
			inline const Renderer::GL::GL &		  getRendererGL() const { return *_rendererGL; }
			inline Renderer::RayTracer &		  getRendererRT() { return *_rendererRT; }
			inline const Renderer::RayTracer &	  getRendererRT() const { return *_rendererRT; }

			void initializeGL() override;
			void paintGL() override;
			void resizeGL( int, int ) override;

			void setRenderMode() { _renderer = nullptr; }
			void activeVSync( const bool p_active );

			const Vec2i getPickedIds( const uint, const uint );

			const ID::VTX_ID getEventFirererId() const override { return ID::UI::Input::OPENGL_WIDGET; }

		  protected:
			OpenGLWidget( QWidget * p_parent );

			void _setupUi( const QString & p_name ) override {}
			void _setupSlots() override {}

			void mouseMoveEvent( QMouseEvent * p_event ) override { _fireEventInput( p_event ); }
			void mousePressEvent( QMouseEvent * p_event ) override { _fireEventInput( p_event ); }
			void mouseReleaseEvent( QMouseEvent * p_event ) override { _fireEventInput( p_event ); }
			void mouseDoubleClickEvent( QMouseEvent * p_event ) override { _fireEventInput( p_event ); }
			void keyPressEvent( QKeyEvent * p_event ) override { _fireEventInput( p_event ); }
			void keyReleaseEvent( QKeyEvent * p_event ) override { _fireEventInput( p_event ); }
			void wheelEvent( QWheelEvent * p_event ) override { _fireEventInput( p_event ); }

		  private:
			QElapsedTimer _timer		= QElapsedTimer();
			QElapsedTimer _frameTimer	= QElapsedTimer();
			uint		  _frameCounter = 0u;
			QPainter	  _painter		= QPainter();

			Renderer::BaseRenderer * _renderer	 = nullptr;
			Renderer::GL::GL *		 _rendererGL = nullptr;
			Renderer::RayTracer *	 _rendererRT = nullptr;

			void _switchRenderer( const Renderer::MODE );
		};
	} // namespace UI::Widget::Render

} // namespace VTX

#endif
