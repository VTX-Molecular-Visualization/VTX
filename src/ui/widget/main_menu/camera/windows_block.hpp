#ifndef __VTX_UI_WIDGET_MENU_CAMERA_WINDOWS_BLOCK__
#define __VTX_UI_WIDGET_MENU_CAMERA_WINDOWS_BLOCK__

#include "event/event.hpp"
#include "id.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <QWidget>
#include <map>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Camera
{
	class WindowsBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~WindowsBlock();
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void refresh();

	  protected:
		WindowsBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshButton( const ID::VTX_ID & p_id );
		void _updateFullscreenButton( const WindowMode & p_mode );

	  private:
		// !V0.1
		// MenuToolButtonWidget * _minimap			= nullptr;
		// MenuToolButtonWidget * _infoUnderCursor = nullptr;

		MenuToolButtonWidget * _fullscreen			 = nullptr;
		MenuToolButtonWidget * _windowComboBoxButton = nullptr;
		MenuToolButtonWidget * _settingsButton		 = nullptr;

		QMenu * _windowsMenu = nullptr;

		std::map<const ID::VTX_ID *, QAction *> _mapWindowsActions = std::map<const ID::VTX_ID *, QAction *>();

		void _instantiateButton( const ID::VTX_ID & p_id,
								 void ( WindowsBlock::*p_action )(),
								 const QKeySequence & p_shortcut = 0 );

		void _toggleSceneWindow();
		void _toggleRenderWindow();
		void _toggleConsoleWindow();
		void _toggleInspectorWindow();
		// !V0.1
		// void _toggleSelectionWindow();
		void _toggleSequenceWindow();

		void _displaySettingsWindow();
		void _toggleWindowState() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
