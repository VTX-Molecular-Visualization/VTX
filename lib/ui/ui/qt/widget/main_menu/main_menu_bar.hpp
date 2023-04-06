#ifndef __VTX_UI_QT_MAIN_MENU_BAR__
#define __VTX_UI_QT_MAIN_MENU_BAR__

#include "core/main_menu/main_menu_bar.hpp"
#include "qt/widget/base_manual_widget.hpp"
#include <QMenuBar>
#include <QResizeEvent>
#include <QTabWidget>

namespace VTX::UI::QT::Widget::MainMenu
{
	class MainMenuBar : public Core::MainMenu::MainMenuBar, public BaseManualWidget<QMenuBar>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void		localize() override;
		inline void setCurrentTab( int p_index ) override { _tabWidget->setCurrentIndex( 0 ); };

		Core::MainMenu::MainMenuTooltab * _instantiateTab() const override;

		void _addTab( const Core::WidgetKey & p_tabID, Core::MainMenu::MainMenuTooltab * const p_tab ) override;

	  protected:
		MainMenuBar( QWidget * p_parent );

		void resizeEvent( QResizeEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QTabWidget * _tabWidget = nullptr;
	};
} // namespace VTX::UI::QT::Widget::MainMenu

#endif
