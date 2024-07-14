#ifndef __VTX_UI_QT_OPENGL_WIDGET__
#define __VTX_UI_QT_OPENGL_WIDGET__

#include "qt/base_widget.hpp"
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWindow>

namespace VTX::UI::QT
{
	// Reimplement a custom OpenGL widget to avoid the use of QOpenGLWidget.
	class OpenGLWidget : public QT::BaseWidget<OpenGLWidget, QWidget>
	{
	  public:
		OpenGLWidget( QWidget * p_parent );
		virtual ~OpenGLWidget();

		void render();
		void resizeEvent( QResizeEvent * p_event ) override;

	  private:
		QPointer<QOpenGLContext> _context;
		QPointer<QWindow>		 _surface;
		QOpenGLPaintDevice *	 _device;
		QPointer<QWidget>		 _container;
	};
} // namespace VTX::UI::QT

#endif
