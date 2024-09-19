#ifndef __VTX_UI_QT_WIDGET_OPENGL_WIDGET__
#define __VTX_UI_QT_WIDGET_OPENGL_WIDGET__

#include "ui/qt/base_widget.hpp"
#include "ui/qt/window/event_catch_window.hpp"
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWindow>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Reimplement a custom OpenGL widget to avoid the use of QOpenGLWidget.
	 * This is necessary to avoid makeCurrent() and doneCurrent() by using custom context.
	 */
	class OpenGLWidget : public QT::BaseWidget<OpenGLWidget, QWidget>
	{
	  public:
		OpenGLWidget( QWidget * );
		virtual ~OpenGLWidget();

		void render();
		void resizeEvent( QResizeEvent * ) override;

		//  bool eventFilter( QObject *, QEvent * );

	  private:
		QPointer<QOpenGLContext>		   _context;
		QPointer<Window::EventCatchWindow> _surface;
		QOpenGLPaintDevice *			   _device;
		QPointer<QWidget>				   _container;
	};
} // namespace VTX::UI::QT::Widget

#endif
