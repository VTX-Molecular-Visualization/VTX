#ifndef __VTX_UI_WIDGET_MENU_MAIN_SESSION__
#define __VTX_UI_WIDGET_MENU_MAIN_SESSION__

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
			class MenuMainSessionWidget : public MenuToolBlockWidget
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~MenuMainSessionWidget();
				void localize() override;

			  protected:
				MenuMainSessionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
				void setupUi( const QString & p_name ) override;
				void setupSlots() override;

			  private:
				MenuToolButtonWidget * _newSessionButton		= nullptr;
				MenuToolButtonWidget * _openSessionButton		= nullptr;
				MenuToolButtonWidget * _openRecentSessionButton = nullptr;
				MenuToolButtonWidget * _saveSessionButton		= nullptr;
				MenuToolButtonWidget * _saveAsSessionButton		= nullptr;

			  private:
				void _openFile();
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
