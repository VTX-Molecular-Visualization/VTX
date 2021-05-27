#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECT_BUTTON__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_RENDER_EFFECT_BUTTON__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"

namespace VTX::UI::Widget::MainMenu
{
	class RenderEffectPresetButton : public MenuToolButtonWidget
	{
		VTX_WIDGET

	  public:
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void setRenderEffectID( const int p_id ) { _id = p_id; };

	  protected:
		RenderEffectPresetButton( QWidget * p_parent ) : MenuToolButtonWidget( p_parent ) {};

	  private:
		int _id = -1;

		void _onButtonClicked();
	};
} // namespace VTX::UI::Widget::MainMenu
#endif
