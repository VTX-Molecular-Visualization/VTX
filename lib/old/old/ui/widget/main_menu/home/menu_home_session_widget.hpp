#ifndef __VTX_UI_WIDGET_MENU_HOME_SESSION__
#define __VTX_UI_WIDGET_MENU_HOME_SESSION__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QMenu>
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Home
{
	class MenuHomeSessionWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuHomeSessionWidget();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuHomeSessionWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		MenuToolButtonWidget * _newSessionButton		= nullptr;
		MenuToolButtonWidget * _downloadMoleculeButton	= nullptr;
		MenuToolButtonWidget * _openSessionButton		= nullptr;
		MenuToolButtonWidget * _openRecentSessionButton = nullptr;
		MenuToolButtonWidget * _saveSessionButton		= nullptr;
		MenuToolButtonWidget * _saveAsSessionButton		= nullptr;

		QMenu * _recentSessionMenu = nullptr;

	  private:
		void _newSession() const;
		void _downloadMoleculeFile() const;
		void _openFile() const;
		void _loadRecentSession( const int & p_ptrSessionIndex ) const;
		void _saveSession() const;
		void _saveAsSession() const;

		void _refreshRecentFiles();
	};
} // namespace VTX::UI::Widget::MainMenu::Home
#endif
