#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION_BUTTON__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION_BUTTON__

#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"

namespace VTX::UI::Widget::MainMenu
{
	class RepresentationPresetButton : public MenuToolButtonWidget
	{
		VTX_WIDGET

	  public:
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void setRepresentationID( const int p_id ) { _id = p_id; };

	  protected:
		RepresentationPresetButton( QWidget * p_parent ) : MenuToolButtonWidget( p_parent ) {};

	  private:
		int _id = -1;

		void _onButtonClicked();
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
