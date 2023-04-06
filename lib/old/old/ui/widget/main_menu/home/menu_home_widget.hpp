#ifndef __VTX_UI_WIDGET_MENU_HOME__
#define __VTX_UI_WIDGET_MENU_HOME__

#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Home
{
	class MenuHomeWidget : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~MenuHomeWidget();
		void localize() override;

	  protected:
		MenuHomeWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::Widget::MainMenu::Home
#endif
