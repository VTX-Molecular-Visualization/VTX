#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_menutoolblock_widget.hpp"
#include "ui_menu_main_session_widget.h"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainSessionWidget : public BaseMenuToolBlockWidget<QWidget, Ui_MenuMainSessionWidget>
			{
				Q_OBJECT

			  public:
				MenuMainSessionWidget( QWidget * p_parent ) : BaseMenuToolBlockWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
