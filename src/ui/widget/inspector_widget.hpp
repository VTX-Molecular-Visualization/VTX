#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QDockWidget>
#include <QVBoxLayout>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class InspectorWidget : public BaseManualWidget<QDockWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~InspectorWidget();
				void localize() override;
				void receiveEvent( const Event::VTXEvent & p_event ) override;
				void refresh();
				void clear();

			  protected:
				InspectorWidget( QWidget * p_parent );

				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				QVBoxLayout * _verticalLayout = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
