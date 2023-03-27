#include "windows_block.hpp"
#include "action/main.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	WindowsBlock::WindowsBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
		_registerEvent( Event::Global::MAIN_WINDOW_MODE_CHANGE );
		_registerEvent( Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE );
	}

	WindowsBlock::~WindowsBlock() {}

	void WindowsBlock::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			refresh();
		}
		else if ( p_event.name == Event::Global::MAIN_WINDOW_MODE_CHANGE )
		{
			const WindowMode mode = dynamic_cast<const Event::VTXEventValue<WindowMode> &>( p_event ).value;
			_updateFullscreenButton( mode );
		}
		else if ( p_event.name == Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE )
		{
			_refreshOverlayVisibilityMenu();
		}
	}

	void WindowsBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_windowComboBoxButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "windowsButton" );
		_windowComboBoxButton->setData( "Panels", ":/sprite/windows_icon.png", Qt::Orientation::Vertical );
		pushButton( *_windowComboBoxButton, 0 );

		_overlaysMenu = new QMenu( this );
		Util::UI::fillMenu( *_overlaysMenu,
							int( Widget::Render::Overlay::OVERLAY::COUNT ),
							Widget::Render::Overlay::OVERLAY_NAMES_STR,
							true );

		MenuToolButtonWidget * const overlaysComboBoxButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "overlaysComboBoxButton" );
		overlaysComboBoxButton->setData( "Overlays", ":/sprite/overlays_icon.png", Qt::Orientation::Vertical );
		overlaysComboBoxButton->setMenu( _overlaysMenu );
		pushButton( *overlaysComboBoxButton, 1 );

		_settingsButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "settingsButton" );
		_settingsButton->setData( "Settings", ":/sprite/settings_icon.png", Qt::Orientation::Vertical );
		pushButton( *_settingsButton, 2 );

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
		pushButton( *_fullscreen, 3 );

		// !V0.1
		//_infoUnderCursor
		//	= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showInfoUnderCursorButton" );
		//_infoUnderCursor->setData( "Show\nCursor Info", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal
		//); pushButton( *_infoUnderCursor, 1 );

		//_minimap = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showMinimapButton" );
		//_minimap->setData( "Show\nMinimap", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		// pushButton( *_minimap, 2 );

		//_refreshOverlayVisibilityMenu();

		validate();
	}
	void WindowsBlock::_setupSlots()
	{
		connect( _settingsButton, &MenuToolButtonWidget::clicked, this, &WindowsBlock::_displaySettingsWindow );
		_fullscreen->setTriggerAction( this, &WindowsBlock::_toggleWindowState );

		connect( _overlaysMenu, &QMenu::triggered, this, &WindowsBlock::_toggleOverlayVisibilityAction );
	}
	void WindowsBlock::localize() { setTitle( "Windows" ); }
	void WindowsBlock::refresh()
	{
		for ( const std::pair<const ID::VTX_ID * const, QAction *> & pair : _mapWindowsActions )
			_refreshButton( *pair.first );
		_refreshOverlayVisibilityMenu();
	}
	void WindowsBlock::_refreshButton( const ID::VTX_ID & p_id )
	{
		const bool	windowVisibility = VTXApp::get().getMainWindow().getWidgetVisibility( p_id );
		std::string windowName		 = p_id;
		std::transform( ++windowName.begin(), windowName.end(), ++windowName.begin(), ::tolower );

		_mapWindowsActions[ &p_id ]->setChecked( windowVisibility );
		//_mapWindowsActions[ &p_id ]->setIcon( *Style::IconConst::get().getWindowIcon( p_id ) );
	}

	void WindowsBlock::_refreshOverlayVisibilityMenu() const
	{
		const Render::RenderWidget & renderWigdet
			= VTXApp::get().getMainWindow().getWidget<Render::RenderWidget>( ID::UI::Window::RENDER );

		for ( QAction * const action : _overlaysMenu->actions() )
		{
			const int					   actionData = action->data().toInt();
			const Render::Overlay::OVERLAY overlay	  = Render::Overlay::OVERLAY( actionData );
			const bool					   checkState = renderWigdet.getOverlay( overlay )->isVisible();
			action->setChecked( checkState );
		}
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
	void WindowsBlock::_toggleOverlayVisibilityAction( QAction * const p_action )
	{
		Widget::Render::RenderWidget & renderWidget
			= VTXApp::get().getMainWindow().getWidget<Widget::Render::RenderWidget>( ID::UI::Window::RENDER );

		const int actionData = p_action->data().toInt();

		const Render::Overlay::OVERLAY				 overlayEnum	 = Widget::Render::Overlay::OVERLAY( actionData );
		Widget::Render::Overlay::BaseOverlay * const overlayWidget	 = renderWidget.getOverlay( overlayEnum );
		const bool									 newVisibleState = !overlayWidget->isVisible();

		renderWidget.setOverlayVisibility( overlayEnum, newVisibleState );
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
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Windowed ) );
		else
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Fullscreen ) );
	}
} // namespace VTX::UI::Widget::MainMenu::Camera
