#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_visualization_representation_button_widget.hpp"
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
					class MenuVisualizationRepresentationWidget : public MenuToolBlockWidget
					{
						VTX_WIDGET

					  public:
						~MenuVisualizationRepresentationWidget();
						void localize() override;

					  protected:
						MenuVisualizationRepresentationWidget( QWidget * p_parent );
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						RepresentationPresetButton * _preset1 = nullptr;
						RepresentationPresetButton * _preset2 = nullptr;
						RepresentationPresetButton * _preset3 = nullptr;

						RepresentationPresetButton * _preset4 = nullptr;
						RepresentationPresetButton * _preset5 = nullptr;
						RepresentationPresetButton * _preset6 = nullptr;

						MenuToolButtonWidget * _addPreset = nullptr;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
