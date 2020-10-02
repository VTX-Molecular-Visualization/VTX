#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "opengl_widget.hpp"
#include "render_widget.hpp"
#include <QDockWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class RenderWidget : public BaseManualWidget<QDockWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~RenderWidget();
				void localize() override;

			  protected:
				RenderWidget( QWidget * p_parent );

				void setupUi( const QString & p_name ) override;
				void setupSlots() override;

			  private:
				OpenGLWidget * _openGLWidget = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
