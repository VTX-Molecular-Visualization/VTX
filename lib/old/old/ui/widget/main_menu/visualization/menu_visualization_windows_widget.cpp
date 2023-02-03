#include "menu_visualization_windows_widget.hpp"
#include "action/main.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationWindowsWidget::MenuVisualizationWindowsWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
		_registerEvent( Event::Global::MAIN_WINDOW_MODE_CHANGE );
	}

	MenuVisualizationWindowsWidget::~MenuVisualizationWindowsWidget() {}

	void MenuVisualizationWindowsWidget::receiveEvent( const Event::VTXEvent & p_event )
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
	}

	void MenuVisualizationWindowsWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_windowComboBoxButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "windowsButton" );
		_windowComboBoxButton->setData( "Display", ":/sprite/windows_icon.png", Qt::Orientation::Vertical );
		pushButton( *_windowComboBoxButton, 0 );

		_settingsButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "settingsButton" );
		_settingsButton->setData( "Settings", ":/sprite/settings_icon.png", Qt::Orientation::Vertical );
		pushButton( *_settingsButton, 1 );

		_windowsMenu = new QMenu( this );

		_instantiateButton( ID::UI::Window::RENDER, &MenuVisualizationWindowsWidget::_toggleRenderWindow );
		_instantiateButton( ID::UI::Window::SCENE, &MenuVisualizationWindowsWidget::_toggleSceneWindow );
		_instantiateButton( ID::UI::Window::INSPECTOR, &MenuVisualizationWindowsWidget::_toggleInspectorWindow );
		// !V0.1
		//_instantiateButton( ID::UI::Window::SELECTION, &MenuVisualizationWindowsWidget::_toggleSelectionWindow );
		_instantiateButton( ID::UI::Window::SEQUENCE, &MenuVisualizationWindowsWidget::_toggleSequenceWindow );
		_instantiateButton( ID::UI::Window::CONSOLE, &MenuVisualizationWindowsWidget::_toggleConsoleWindow );

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
	void MenuVisualizationWindowsWidget::_setupSlots()
	{
		connect( _settingsButton,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationWindowsWidget::_displaySettingsWindow );
		_fullscreen->setTriggerAction( this, &MenuVisualizationWindowsWidget::_toggleWindowState );
	}
	void MenuVisualizationWindowsWidget::localize() { setTitle( "Windows" ); }
	void MenuVisualizationWindowsWidget::refresh()
	{
		for ( const std::pair<const ID::VTX_ID * const, QAction *> & pair : _mapWindowsActions )
			_refreshButton( *pair.first );
	}
	void MenuVisualizationWindowsWidget::_refreshButton( const ID::VTX_ID & p_id )
	{
		const bool	windowVisibility = VTXApp::get().getMainWindow().getWidgetVisibility( p_id );
		std::string windowName		 = p_id;
		std::transform( ++windowName.begin(), windowName.end(), ++windowName.begin(), ::tolower );

		_mapWindowsActions[ &p_id ]->setChecked( windowVisibility );
		//_mapWindowsActions[ &p_id ]->setIcon( *Style::IconConst::get().getWindowIcon( p_id ) );
	}

	void MenuVisualizationWindowsWidget::_instantiateButton( const ID::VTX_ID & p_id,
															 void ( MenuVisualizationWindowsWidget::*p_action )(),
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

	void MenuVisualizationWindowsWidget::_updateFullscreenButton( const WindowMode & p_mode )
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

	void MenuVisualizationWindowsWidget::_toggleSceneWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SCENE );
	}
	void MenuVisualizationWindowsWidget::_toggleRenderWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::RENDER );
	}
	void MenuVisualizationWindowsWidget::_toggleConsoleWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::CONSOLE );
	}
	void MenuVisualizationWindowsWidget::_toggleInspectorWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::INSPECTOR );
	}
	// !V0.1
	// void MenuVisualizationWindowsWidget::_toggleSelectionWindow()
	//{
	//	VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SELECTION );
	//}
	void MenuVisualizationWindowsWidget::_toggleSequenceWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SEQUENCE );
	}

	void MenuVisualizationWindowsWidget::_displaySettingsWindow()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Settings::SETTING_MENU::VTX );
	}
	void MenuVisualizationWindowsWidget::_toggleWindowState() const
	{
		const Qt::WindowStates windowState = VTXApp::get().getMainWindow().windowState();

		if ( windowState & Qt::WindowStates::enum_type::WindowFullScreen )
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Windowed ) );
		else
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Fullscreen ) );
	}
} // namespace VTX::UI::Widget::MainMenu::Visualization
