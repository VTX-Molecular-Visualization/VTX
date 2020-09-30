#ifndef __VTX_UI_WIDGET_MENU_MAIN__
#define __VTX_UI_WIDGET_MENU_MAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_menu_main_widget.h"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuMainWidget : public BaseWidget<QWidget, Ui_MenuMainWidget>
			{
				Q_OBJECT

			  public:
				MenuMainWidget( QWidget * p_parent = nullptr ) : BaseWidget( p_parent ) {};

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
