#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_VISUAL_MODEL__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_VISUAL_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					class MenuVisualizationVisualModelWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationVisualModelWidget();
						void localize() override;

					  protected:
						MenuVisualizationVisualModelWidget( QWidget * p_parent );
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						MenuToolButtonWidget * _preset1 = nullptr;
						MenuToolButtonWidget * _preset2 = nullptr;
						MenuToolButtonWidget * _preset3 = nullptr;

						MenuToolButtonWidget * _preset4 = nullptr;
						MenuToolButtonWidget * _preset5 = nullptr;
						MenuToolButtonWidget * _preset6 = nullptr;

						MenuToolButtonWidget * _addPreset = nullptr;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
