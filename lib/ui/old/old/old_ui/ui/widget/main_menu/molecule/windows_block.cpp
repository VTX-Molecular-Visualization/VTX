#include "ui/old_ui/ui/widget/main_menu/molecule/windows_block.hpp"
#include "ui/old_ui/action/setting.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/old/action/main.hpp>
#include <app/old/event/global.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	WindowsBlock::WindowsBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
		_registerEvent( VTX::App::Old::Event::Global::MAIN_WINDOW_MODE_CHANGE );
	}

	WindowsBlock::~WindowsBlock() {}

	void WindowsBlock::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			refresh();
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::MAIN_WINDOW_MODE_CHANGE )
		{
			const WindowMode mode
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<WindowMode> &>( p_event ).get();
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

		_instantiateButton( UI::ID::Window::RENDER, &WindowsBlock::_toggleRenderWindow );
		_instantiateButton( UI::ID::Window::SCENE, &WindowsBlock::_toggleSceneWindow );
		_instantiateButton( UI::ID::Window::INSPECTOR, &WindowsBlock::_toggleInspectorWindow );
		// !V0.1
		//_instantiateButton( UI::ID::Window::SELECTION, &WindowsBlock::_toggleSelectionWindow );
		_instantiateButton( UI::ID::Window::SEQUENCE, &WindowsBlock::_toggleSequenceWindow );
		_instantiateButton( UI::ID::Window::CONSOLE, &WindowsBlock::_toggleConsoleWindow );

		_windowComboBoxButton->setMenu( _windowsMenu );

		_fullscreen = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleFullscreenButton" );
		_fullscreen->setData( "Fullscreen", ":/sprite/fullscreen_icon.png", Qt::Orientation::Vertical );
		_updateFullscreenButton( UI::VTXApp::get().getMainWindow().getWindowMode() );
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
		for ( const std::pair<const App::Old::VTX_ID * const, QAction *> & pair : _mapWindowsActions )
			_refreshButton( *pair.first );
	}
	void WindowsBlock::_refreshButton( const App::Old::VTX_ID & p_id )
	{
		const bool	windowVisibility = UI::VTXApp::get().getMainWindow().getWidgetVisibility( p_id );
		std::string windowName		 = p_id;
		std::transform( ++windowName.begin(), windowName.end(), ++windowName.begin(), ::tolower );

		_mapWindowsActions[ &p_id ]->setChecked( windowVisibility );
		//_mapWindowsActions[ &p_id ]->setIcon( *Style::IconConst::get().getWindowIcon( p_id ) );
	}

	void WindowsBlock::_instantiateButton( const App::Old::VTX_ID & p_id,
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

	void WindowsBlock::_toggleSceneWindow() { UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::SCENE ); }
	void WindowsBlock::_toggleRenderWindow() { UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::RENDER ); }
	void WindowsBlock::_toggleConsoleWindow() { UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::CONSOLE ); }
	void WindowsBlock::_toggleInspectorWindow()
	{
		UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::INSPECTOR );
	}
	// !V0.1
	// void WindowsBlock::_toggleSelectionWindow()
	//{
	//	UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::SELECTION );
	//}
	void WindowsBlock::_toggleSequenceWindow()
	{
		UI::VTXApp::get().getMainWindow().toggleWidget( UI::ID::Window::SEQUENCE );
	}

	void WindowsBlock::_displaySettingsWindow()
	{
		UI::VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::VTX );
	}
	void WindowsBlock::_toggleWindowState() const
	{
		const Qt::WindowStates windowState = UI::VTXApp::get().getMainWindow().windowState();

		if ( windowState & Qt::WindowStates::enum_type::WindowFullScreen )
			VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Windowed ) );
		else
			VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Fullscreen ) );
	}
} // namespace VTX::UI::Widget::MainMenu::Molecule
