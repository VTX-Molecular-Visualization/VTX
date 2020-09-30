#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION_OPEN__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_menu_main_session_open_widget.h"
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainSessionOpenWidget : public BaseWidget<QToolButton, Ui_MenuMainSessionOpenWidget>
			{
				Q_OBJECT

			  public:
				MenuMainSessionOpenWidget( QWidget * p_parent ) : BaseWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
