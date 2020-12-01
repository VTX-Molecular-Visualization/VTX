#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION_BUTTON__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION_BUTTON__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				class RepresentationPresetButton : public MenuToolButtonWidget
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					RepresentationPresetButton( QWidget * p_parent ) : MenuToolButtonWidget( p_parent ) {};
					void _setupSlots() override
					{
						MenuToolButtonWidget::_setupSlots();
						connect( this, &QToolButton::clicked, this, &RepresentationPresetButton::_onButtonClicked );
					}
					void setRepresentationID( const int p_id ) { _id = p_id; };

				  private:
					int _id;

					void _onButtonClicked();
				};
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
