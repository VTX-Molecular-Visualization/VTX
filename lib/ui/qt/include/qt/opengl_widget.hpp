#ifndef __VTX_UI_QT_OPENGL_WIDGET__
#define __VTX_UI_QT_OPENGL_WIDGET__

// Before QOpenGLWidget to use glad instead of QOpenGLFunctions.
#include "qt/base_widget.hpp"
#include <QOpenGLWidget>
#include <QWidget>
#include <renderer/facade.hpp>

namespace VTX
{
	namespace UI::QT
	{
		class OpenGLWidget : public QT::BaseWidget<OpenGLWidget, QOpenGLWidget>
		{
		  public:
			// using CallbackInitGL   = std::function<void()>;
			// using CallbackResizeGL = std::function<void( int, int )>;

			OpenGLWidget( QWidget * p_parent );
			~OpenGLWidget();

			void initializeGL() override;
			void paintGL() override;
			void resizeGL( int, int ) override;

			// inline void setCallbackInitGL( const CallbackInitGL & p_cb ) { _cbInitGL = p_cb; }

		  private:
			int _makeCurrentCounter = 0;
		};
	} // namespace UI::QT

} // namespace VTX

#endif
