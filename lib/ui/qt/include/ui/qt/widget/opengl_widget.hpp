#ifndef __VTX_UI_QT_WIDGET_OPENGL_WIDGET__
#define __VTX_UI_QT_WIDGET_OPENGL_WIDGET__

#include "ui/qt/core/base_widget.hpp"
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
	class OpenGLWidget : public QT::Core::BaseWidget<OpenGLWidget, QWidget>
	{
	  public:
		OpenGLWidget( QWidget * );
		~OpenGLWidget();

		void render();
		void resizeEvent( QResizeEvent * ) override;

		//  bool eventFilter( QObject *, QEvent * );

		void setVSync( const bool );
		bool isVSync() const { return _context->format().swapInterval() == 1; }

	  protected:
		bool eventFilter( QObject *, QEvent * ) override;

	  private:
		QPointer<QOpenGLContext>		   _context;
		QPointer<Window::EventCatchWindow> _window;
		QPointer<QWidget>				   _container;
	};
} // namespace VTX::UI::QT::Widget

#endif
