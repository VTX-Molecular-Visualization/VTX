#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION_OPENRECENT__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION_OPENRECENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_menu_main_session_openrecent_widget.h"
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainSessionOpenRecentWidget : public BaseWidget<QToolButton, Ui_MenuMainSessionOpenRecentWidget>
			{
				Q_OBJECT

			  public:
				MenuMainSessionOpenRecentWidget( QWidget * p_parent = nullptr ) : BaseWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
