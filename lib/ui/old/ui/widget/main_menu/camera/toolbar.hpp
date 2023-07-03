#ifndef __VTX_UI_WIDGET_MENU_CAMERA__
#define __VTX_UI_WIDGET_MENU_CAMERA__

#include "ui/old_ui/ui/widget/main_menu/menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class Toolbar : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~Toolbar();
		void localize() override;

	  protected:
		Toolbar( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
