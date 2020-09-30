#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION_SAVEAS__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION_SAVEAS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_menutoolbutton_widget.hpp"
#include "ui_menu_main_session_saveas_widget.h"
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainSessionSaveAsWidget : public BaseMenuToolButtonWidget<Ui_MenuMainSessionSaveAsWidget>
			{
				Q_OBJECT

			  public:
				MenuMainSessionSaveAsWidget( QWidget * p_parent = nullptr ) : BaseMenuToolButtonWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
