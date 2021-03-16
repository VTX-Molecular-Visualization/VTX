#ifndef __VTX_UI_WIDGET_MENU_EXTENSIONS__
#define __VTX_UI_WIDGET_MENU_EXTENSIONS__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_extensions_all_widget.hpp"
#include "menu_extensions_jobs.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Extensions
				{
					class MenuExtensionsWidget : public MenuTooltabWidget
					{
						VTX_WIDGET

					  public:
						~MenuExtensionsWidget();
						void localize() override;

					  protected:
						MenuExtensionsWidget( QWidget * p_parent = nullptr );
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						MenuExtensionsAllWidgets *	_allExtensions = nullptr;
						MenuExtensionsJobsWidgets * _jobs		   = nullptr;
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
