#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION_NEW__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION_NEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_menutoolbutton_widget.hpp"
#include "ui_menu_main_session_new_widget.h"
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainSessionNewWidget : public BaseMenuToolButtonWidget<Ui_MenuMainSessionNewWidget>
			{
				Q_OBJECT

			  public:
				MenuMainSessionNewWidget( QWidget * p_parent = nullptr ) : BaseMenuToolButtonWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
