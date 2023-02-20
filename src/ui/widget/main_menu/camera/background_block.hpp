#ifndef __VTX_UI_WIDGET_MENU_CAMERA_BACKGROUND_BLOCK__
#define __VTX_UI_WIDGET_MENU_CAMERA_BACKGROUND_BLOCK__

#include "color/rgba.hpp"
#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	class BackgroundBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~BackgroundBlock();
		void localize() override;

	  protected:
		BackgroundBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Default backgrounds
		MenuToolButtonWidget * _backgroundBlackButton = nullptr;
		MenuToolButtonWidget * _backgroundWhiteButton = nullptr;

		void _setBackgroundToBlack() const;
		void _setBackgroundToWhite() const;
		void _setBackgroundColor( const Color::Rgba & p_color ) const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
