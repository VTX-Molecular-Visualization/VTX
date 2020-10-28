#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_OBJECT_DISPLAY__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_OBJECT_DISPLAY__

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
					class MenuVisualizationObjectDisplayWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationObjectDisplayWidget();
						void localize() override;

					  protected:
						MenuVisualizationObjectDisplayWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Show / Hide Molecular Objects
						MenuToolButtonWidget * _showWater	  = nullptr;
						MenuToolButtonWidget * _showSolvant	  = nullptr;
						MenuToolButtonWidget * _showHydrogens = nullptr;
					}; // namespace Visualization
				}	   // namespace Visualization
			}		   // namespace MainMenu
		}			   // namespace Widget
	}				   // namespace UI
} // namespace VTX
#endif