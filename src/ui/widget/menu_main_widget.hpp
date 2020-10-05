#ifndef __VTX_UI_WIDGET_MENU_MAIN__
#define __VTX_UI_WIDGET_MENU_MAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "menu_main_session_widget.hpp"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainWidget : public BaseManualWidget<QWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~MenuMainWidget();
				void localize() override;

			  protected:
				MenuMainWidget( QWidget * p_parent = nullptr );
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				QHBoxLayout * _horizontalLayout = nullptr;
				QSpacerItem * _horizontalSpacer = nullptr;

				MenuMainSessionWidget * _session = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
