#include "menu_visualization_windows_widget.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationWindowsWidget::MenuVisualizationWindowsWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
	}

	MenuVisualizationWindowsWidget::~MenuVisualizationWindowsWidget() {}

	void MenuVisualizationWindowsWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			refresh();
		}
	}

	void MenuVisualizationWindowsWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_windowComboBoxButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "windowsButton" );
		_windowComboBoxButton->setData( "Windows", ":/sprite/windows_icon.png", Qt::Orientation::Vertical );
		pushButton( *_windowComboBoxButton, 0 );

		_windowsMenu = new QMenu( this );

		_instantiateButton( ID::UI::Window::RENDER, &MenuVisualizationWindowsWidget::_toggleRenderWindow );
		_instantiateButton( ID::UI::Window::SCENE, &MenuVisualizationWindowsWidget::_toggleSceneWindow );
		_instantiateButton( ID::UI::Window::INSPECTOR, &MenuVisualizationWindowsWidget::_toggleInspectorWindow );
		_instantiateButton( ID::UI::Window::SELECTION, &MenuVisualizationWindowsWidget::_toggleSelectionWindow );
		_instantiateButton( ID::UI::Window::SEQUENCE, &MenuVisualizationWindowsWidget::_toggleSequenceWindow );
		_instantiateButton( ID::UI::Window::CONSOLE, &MenuVisualizationWindowsWidget::_toggleConsoleWindow );
		_instantiateButton( ID::UI::Window::SETTINGS, &MenuVisualizationWindowsWidget::_toggleSettingWindow );

		_windowComboBoxButton->setMenu( _windowsMenu );

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
	void MenuVisualizationWindowsWidget::_setupSlots() {}
	void MenuVisualizationWindowsWidget::localize() { setTitle( "Windows" ); }
	void MenuVisualizationWindowsWidget::refresh()
	{
		for ( const std::pair<const ID::VTX_ID *, QAction *> & pair : _mapWindowsActions )
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
	void MenuVisualizationWindowsWidget::_toggleSettingWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SETTINGS );
	}
	void MenuVisualizationWindowsWidget::_toggleSelectionWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SELECTION );
	}
	void MenuVisualizationWindowsWidget::_toggleSequenceWindow()
	{
		VTXApp::get().getMainWindow().toggleWidget( ID::UI::Window::SEQUENCE );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
