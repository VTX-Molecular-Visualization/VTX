#ifndef __VTX_UI_WIDGET_INSPECTOR__
#define __VTX_UI_WIDGET_INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QDockWidget>
#include <QListWidget>

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

			  protected:
				InspectorWidget( QWidget * p_parent );

				void setupUi( const QString & p_name ) override;
				void setupSlots() override;

			  private:
				QListWidget * _listWidget = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
