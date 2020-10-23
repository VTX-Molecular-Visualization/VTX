#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__

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
					class MenuVisualizationWindowsWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationWindowsWidget();
						void localize() override;

					  protected:
						MenuVisualizationWindowsWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Render view tools
						MenuToolButtonWidget * _minimap			= nullptr;
						MenuToolButtonWidget * _infoUnderCursor = nullptr;
						MenuToolButtonWidget * _sequence		= nullptr;

						void _openSequenceWindow();
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
