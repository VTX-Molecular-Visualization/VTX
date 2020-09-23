#ifndef __VTX_UI_OPENGL_WIDGET__
#define __VTX_UI_OPENGL_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include <QOpenGLWidget>

namespace VTX
{
	namespace UI
	{
		class OpenGLWidget : public QOpenGLWidget
		{
			Q_OBJECT

		  public:
			OpenGLWidget( QWidget * = 0 );
			~OpenGLWidget();

		  signals:

		  public slots:

		  private:
		};

	} // namespace UI
} // namespace VTX

#endif
