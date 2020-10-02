#ifndef __VTX_UI_WIDGET_OPENGL__
#define __VTX_UI_WIDGET_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_opengl_widget.h"
#include <QOpenGLFunctions_4_5_Compatibility>
#include <QOpenGLWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_5_Compatibility
			{
			  public:
				OpenGLWidget( QWidget * p_parent = 0 );
				~OpenGLWidget();

				void initializeGL() override;
				void paintGL() override;
				void resizeGL( int, int ) override;

			  private:
				// QSurfaceFormat * _format  = new QSurfaceFormat();
				// QOpenGLContext * _context = new QOpenGLContext();
				// QOpenGLPaintDevice * _device  = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
