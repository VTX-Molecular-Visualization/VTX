#ifndef __VTX_UI_QT_WIDGET_OPENGL_WIDGET__
#define __VTX_UI_QT_WIDGET_OPENGL_WIDGET__

#include "ui/qt/base_widget.hpp"
#include "ui/qt/helper.hpp"
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWindow>
#include <app/core/input/input_manager.hpp>

namespace VTX::UI::QT::Widget
{

	// Reimplement a custom OpenGL widget to avoid the use of QOpenGLWidget.
	class OpenGLWidget : public QT::BaseWidget<OpenGLWidget, QWidget>
	{
	  public:
		OpenGLWidget( QWidget * p_parent );
		virtual ~OpenGLWidget();

		void render();
		void resizeEvent( QResizeEvent * p_event ) override;

		//  bool eventFilter( QObject *, QEvent * );

	  private:
		class OpenGLWindow : public QWindow
		{
		  public:
			inline void keyPressEvent( QKeyEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
			}

			inline void keyReleaseEvent( QKeyEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
			}

			inline void mousePressEvent( QMouseEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			}

			inline void mouseReleaseEvent( QMouseEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			}

			inline void mouseDoubleClickEvent( QMouseEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			}

			inline void mouseMoveEvent( QMouseEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			}

			inline void wheelEvent( QWheelEvent * const p_event ) override
			{
				App::INPUT_MANAGER().handleMouseWheelEvent( Helper::qWheelEventToWheelEvent( *p_event ) );
			}
		};

		QPointer<QOpenGLContext> _context;
		QPointer<OpenGLWindow>	 _surface;
		QOpenGLPaintDevice *	 _device;
		QPointer<QWidget>		 _container;
	};
} // namespace VTX::UI::QT::Widget

#endif
