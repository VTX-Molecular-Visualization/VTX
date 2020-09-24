#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_inspector_widget.h"
#include <QDockWidget>


namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class InspectorWidget : public BaseWidget<QDockWidget, Ui_InspectorWidget>
			{
				Q_OBJECT

			  public:
				InspectorWidget( QWidget * p_parent );

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  private:
				Ui_InspectorWidget * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
