#ifndef __VTX_UI_WIDGET_MENU_HOME_WINDOWS__
#define __VTX_UI_WIDGET_MENU_HOME_WINDOWS__

#include <app/old_app/event/event.hpp>
#include <app/old_app/id.hpp>
#include "old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <map>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Home
{
	class MenuHomeWindowsWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuHomeWindowsWidget();
		void localize() override;
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;
		void refresh();

	  protected:
		MenuHomeWindowsWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshButton( const ID::VTX_ID & p_id );
		void _updateFullscreenButton( const WindowMode & p_mode );

	  private:
		MenuToolButtonWidget * _windowComboBoxButton = nullptr;
		MenuToolButtonWidget * _fullscreen			 = nullptr;
		MenuToolButtonWidget * _settingsButton		 = nullptr;
		MenuToolButtonWidget * _quitButton			 = nullptr;

		QMenu * _windowsMenu = nullptr;

		std::map<const ID::VTX_ID *, QAction *> _mapWindowsActions = std::map<const ID::VTX_ID *, QAction *>();

		void _instantiateButton( const ID::VTX_ID & p_id,
								 void ( MenuHomeWindowsWidget::*p_action )(),
								 const QKeySequence & p_shortcut = 0 );

		void _toggleSceneWindow();
		void _toggleRenderWindow();
		void _toggleConsoleWindow();
		void _toggleInspectorWindow();
		// !V0.1
		// void _toggleSelectionWindow();
		void _toggleSequenceWindow();

		void _toggleWindowState() const;
		void _displaySettingsWindow();
		void _quit();
	};
} // namespace VTX::UI::Widget::MainMenu::Home
#endif
