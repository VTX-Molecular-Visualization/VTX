#include "windows_block.hpp"
#include "old_ui/action/setting.hpp"
#include "old_ui/ui/main_window.hpp"
#include "old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "old_ui/vtx_app.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/main.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	WindowsBlock::WindowsBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
		_registerEvent( VTX::Event::Global::MAIN_WINDOW_MODE_CHANGE );
	}

	WindowsBlock::~WindowsBlock() {}

	void WindowsBlock::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			refresh();
		}
		else if ( p_event.name == VTX::Event::Global::MAIN_WINDOW_MODE_CHANGE )
		{
			const WindowMode mode = dynamic_cast<const VTX::Event::VTXEventValue<WindowMode> &>( p_event ).value;
			_updateFullscreenButton( mode );
		}
	}

	void WindowsBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_windowComboBoxButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "windowsButton" );
		_windowComboBoxButton->setData( "Panels", ":/sprite/windows_icon.png", Qt::Orientation::Vertical );
		pushButton( *_windowComboBoxButton, 0 );

		_settingsButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "settingsButton" );
		_settingsButton->setData( "Settings", ":/sprite/settings_icon.png", Qt::Orientation::Vertical );
		pushButton( *_settingsButton, 1 );

		_windowsMenu = new QMenu( this );

		_instantiateButton( ID::UI::Window::RENDER, &WindowsBlock::_toggleRenderWindow );
		_instantiateButton( ID::UI::Window::SCENE, &WindowsBlock::_toggleSceneWindow );
		_instantiateButton( ID::UI::Window::INSPECTOR, &WindowsBlock::_toggleInspectorWindow );
		// !V0.1
		//_instantiateButton( ID::UI::Window::SELECTION, &WindowsBlock::_toggleSelectionWindow );
		_instantiateButton( ID::UI::Window::SEQUENCE, &WindowsBlock::_toggleSequenceWindow );
		_instantiateButton( ID::UI::Window::CONSOLE, &WindowsBlock::_toggleConsoleWindow );

		_windowComboBoxButton->setMenu( _windowsMenu );

		_fullscreen = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleFullscreenButton" );
		_fullscreen->setData( "Fullscreen", ":/sprite/fullscreen_icon.png", Qt::Orientation::Vertical );
		_updateFullscreenButton( VTXApp::get().getMainWindow().getWindowMode() );
		pushButton( *_fullscreen, 2 );

		// !V0.1
		//_infoUnderCursor
		//	= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showInfoUnderCursorButton" );
		//_infoUnderCursor->setData( "Show\nCursor Info", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal
		//); pushButton( *_infoUnderCursor, 1 );

		//_minimap = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showMinimapButton" );
		//_minimap->setData( "Show\nMinimap", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		// pushButton( *_minimap, 2 );

		validate();
	}
	void WindowsBlock::_setupSlots()
	{
		connect( _settingsButton, &MenuToolButtonWidget::clicked, this, &WindowsBlock::_displaySettingsWindow );
		_fullscreen->setTriggerAction( this, &WindowsBlock::_toggleWindowState );
	}
	void WindowsBlock::localize() { setTitle( "Windows" ); }
	void WindowsBlock::refresh()
	{
		for ( const std::pair<const ID::VTX_ID * const, QAction *> & pair : _mapWindowsActions )
			_refreshButton( *pair.first );
	}
	void WindowsBlock::_refreshButton( const ID::VTX_ID & p_id )
	{
		const bool	windowVisibility = VTXApp::get().getMainWindow().getWidgetVisibility( p_id );
		std::string windowName		 = p_id;
		std::transform( ++windowName.begin(), windowName.end(), ++windowName.begin(), ::tolower );

		_mapWindowsActions[ &p_id ]->setChecked( windowVisibility );
		//_mapWindowsActions[ &p_id ]->setIcon( *Style::IconConst::get().getWindowIcon( p_id ) );
	}

	void WindowsBlock::_instantiateButton( const ID::VTX_ID & p_id,
										   void ( WindowsBlock::*p_action )(),
										   const QKeySequence & p_shortcut )
	{
		std::string windowName = p_id;
		std::transform( ++windowName.begin(), windowName.end(), ++windowName.begin(), ::tolower );

		QAction * const action = new QAction( _windowsMenu );
		// action->setIcon( *Style::IconConst::get().getWindowIcon( p_id ) );
		action->setText( QString::fromStdString( windowName ) );
		action->setCheckable( true );
		connect( action, &QAction::triggered, this, p_action );
		action->setShortcut( p_shortcut );

		_windowsMenu->addAction( action );

		_mapWindowsActions.emplace( &p_id, action );
	}

	void WindowsBlock::_updateFullscreenButton( const WindowMode & p_mode )
	{
		if ( p_mode == WindowMode::Fullscreen )
		{
			_fullscreen->setIcon( Style::IconConst::get().WINDOWED_ICON );
			_fullscreen->setText( "Window" );
		}
		else
		{
			_fullscreen->setIcon( Style::IconConst::get().FULLSCREEN_ICON );
			_fullscreen->setText( "Fullscreen" );
		}
	}

	void WindowsBlock::_toggleSceneWindow() { VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SCENE ); }
	void WindowsBlock::_toggleRenderWindow() { VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::RENDER ); }
	void WindowsBlock::_toggleConsoleWindow() { VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::CONSOLE ); }
	void WindowsBlock::_toggleInspectorWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::INSPECTOR );
	}
	// !V0.1
	// void WindowsBlock::_toggleSelectionWindow()
	//{
	//	VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SELECTION );
	//}
	void WindowsBlock::_toggleSequenceWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SEQUENCE );
	}

	void WindowsBlock::_displaySettingsWindow()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::VTX );
	}
	void WindowsBlock::_toggleWindowState() const
	{
		const Qt::WindowStates windowState = VTXApp::get().getMainWindow().windowState();

		if ( windowState & Qt::WindowStates::enum_type::WindowFullScreen )
			VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Windowed ) );
		else
			VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Fullscreen ) );
	}
} // namespace VTX::UI::Widget::MainMenu::Molecule
