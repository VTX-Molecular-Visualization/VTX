#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_SELECTION_ACTION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_SELECTION_ACTION__

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
					class MenuVisualizationSelectionActionWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationSelectionActionWidget();
						void localize() override;

					  protected:
						MenuVisualizationSelectionActionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Selection actions 1
						MenuToolButtonWidget * _extract = nullptr;
						MenuToolButtonWidget * _copy	= nullptr;
						MenuToolButtonWidget * _delete	= nullptr;

						// Visibility actions
						MenuToolButtonWidget * _show = nullptr;
						MenuToolButtonWidget * _hide = nullptr;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
