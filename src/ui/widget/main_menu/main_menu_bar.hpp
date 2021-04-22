#ifndef __VTX_UI_WIDGET_MAIN_MENU_BAR__
#define __VTX_UI_WIDGET_MAIN_MENU_BAR__

#ifdef _MSC_VER
#pragma once
#endif

#include "extensions/menu_extensions_widget.hpp"
#include "home/menu_home_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "visualization/menu_visualization_widget.hpp"
#include <QLabel>
#include <QMenuBar>
#include <QTabWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				class MainMenuBar : public BaseManualWidget<QMenuBar>
				{
					VTX_WIDGET

				  public:
					void		localize() override;
					inline void setCurrentTab( int p_index ) { _tabWidget->setCurrentIndex( 0 ); };

				  protected:
					MainMenuBar( QWidget * p_parent );

					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QTabWidget *							 _tabWidget		 = nullptr;
					Home::MenuHomeWidget *					 _mainMenu		 = nullptr;
					Visualization::MenuVisualizationWidget * _viewMenu		 = nullptr;
					QLabel *								 _movieMenu		 = nullptr;
					Extensions::MenuExtensionsWidget *		 _extensionsMenu = nullptr;
				};
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
