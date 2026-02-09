#ifndef __VTX_UI_QT_WIDGET_OPENGL_WIDGET__
#define __VTX_UI_QT_WIDGET_OPENGL_WIDGET__

#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/window/renderer.hpp"
#include <QOpenGLContext>
#include <QPointer>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Reimplement a custom OpenGL widget to avoid the use of QOpenGLWidget.
	 * This is necessary to avoid makeCurrent() and doneCurrent() by using custom context.
	 */
	class OpenGLWidget : public BaseWidget<OpenGLWidget, QWidget>
	{
		Q_OBJECT

	  public:
		OpenGLWidget( QWidget * );
		~OpenGLWidget();

		void render( const App::Events::PostRender & );
		void resizeEvent( QResizeEvent * ) override;

		void setVSync( const bool );

		inline bool isVSync() const { return _context->format().swapInterval() == 1; }

	  protected:
		bool eventFilter( QObject *, QEvent * ) override;
		void onResizeFinished();

	  private:
		QPointer<QOpenGLContext>   _context;
		QPointer<Window::Renderer> _window;
		QPointer<QWidget>		   _container;
		QTimer					   _resizeTimer;
	};
} // namespace VTX::UI::QT::Widget

#endif
