#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_WINDOWS__

#include <app/core/event/vtx_event.hpp>
#include <app/id.hpp>
#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <map>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Molecule
{
	class WindowsBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~WindowsBlock();
		void localize() override;
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
		void refresh();

	  protected:
		WindowsBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshButton( const App::VTX_ID & p_id );
		void _updateFullscreenButton( const WindowMode & p_mode );

	  private:
		// !V0.1
		// MenuToolButtonWidget * _minimap			= nullptr;
		// MenuToolButtonWidget * _infoUnderCursor = nullptr;

		MenuToolButtonWidget * _fullscreen			 = nullptr;
		MenuToolButtonWidget * _windowComboBoxButton = nullptr;
		MenuToolButtonWidget * _settingsButton		 = nullptr;

		QMenu * _windowsMenu = nullptr;

		std::map<const App::VTX_ID *, QAction *> _mapWindowsActions = std::map<const App::VTX_ID *, QAction *>();

		void _instantiateButton( const App::VTX_ID & p_id,
								 void ( WindowsBlock::*p_action )(),
								 const QKeySequence & p_shortcut = 0 );

		void _toggleSceneWindow();
		void _toggleRenderWindow();
		void _toggleConsoleWindow();
		void _toggleInspectorWindow();
		void _toggleSequenceWindow();

		void _displaySettingsWindow();
		void _toggleWindowState() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Molecule
#endif
