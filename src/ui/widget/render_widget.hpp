#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_render_widget.h"
#include <QDockWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class RenderWidget : public BaseWidget<QDockWidget, Ui_RenderWidget>
			{
				Q_OBJECT

			public:
				RenderWidget( QWidget * p_parent );

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			private:
				Ui_RenderWidget * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
