#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_toolblock_widget.hpp"
#include "menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
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
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
