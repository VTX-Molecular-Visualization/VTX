#ifndef __VTX_UI_WIDGET_MAIN_MENU_BAR__
#define __VTX_UI_WIDGET_MAIN_MENU_BAR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "menu_main_widget.hpp"
#include <QMenuBar>
#include <QTabWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MainMenuBar : public BaseManualWidget<QMenuBar>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				void		localize() override;
				inline void setCurrentTab( int p_index ) { _tabWidget->setCurrentIndex( 0 ); };

			  protected:
				MainMenuBar( QWidget * p_parent );

				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				QTabWidget *	 _tabWidget = nullptr;
				MenuMainWidget * _mainMenu	= nullptr;
				QLabel *		 _viewMenu	= nullptr;
				QLabel *		 _movieMenu = nullptr;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
