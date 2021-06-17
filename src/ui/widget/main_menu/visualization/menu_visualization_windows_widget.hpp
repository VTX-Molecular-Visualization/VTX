#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__

#include "event/event.hpp"
#include "id.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <map>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationWindowsWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuVisualizationWindowsWidget();
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void refresh();

	  protected:
		MenuVisualizationWindowsWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshButton( const ID::VTX_ID & p_id );

	  private:
		// !V0.1
		// MenuToolButtonWidget * _minimap			= nullptr;
		// MenuToolButtonWidget * _infoUnderCursor = nullptr;

		MenuToolButtonWidget * _windowComboBoxButton = nullptr;
		MenuToolButtonWidget * _settingsButton		 = nullptr;
		MenuToolButtonWidget * _informationButton	 = nullptr;
		MenuToolButtonWidget * _quitButton			 = nullptr;

		QMenu * _windowsMenu = nullptr;

		std::map<const ID::VTX_ID *, QAction *> _mapWindowsActions = std::map<const ID::VTX_ID *, QAction *>();

		void _instantiateButton( const ID::VTX_ID & p_id,
								 void ( MenuVisualizationWindowsWidget::*p_action )(),
								 const QKeySequence & p_shortcut = 0 );

		void _toggleSceneWindow();
		void _toggleRenderWindow();
		void _toggleConsoleWindow();
		void _toggleInspectorWindow();
		// !V0.1
		// void _toggleSelectionWindow();
		void _toggleSequenceWindow();

		void _displaySettingsWindow();
		void _displayInformationWindow();
		void _quit();
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
