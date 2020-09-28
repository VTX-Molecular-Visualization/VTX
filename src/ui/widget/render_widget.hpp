#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_render_widget.h"
#include <QDockWidget>
//#include <QOpenGLFunctions>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class RenderWidget : public BaseWidget<QDockWidget, Ui_RenderWidget>//, protected QOpenGLFunctions
			{
				Q_OBJECT

			  public:
				RenderWidget( QWidget * p_parent );

			  private:
				//QOpenGLContext *	 _context = nullptr;
				//QOpenGLPaintDevice * _device  = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
