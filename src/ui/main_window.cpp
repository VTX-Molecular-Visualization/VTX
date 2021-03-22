#include "main_window.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "vtx_app.hpp"
#include "widget_factory.hpp"
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <QSize>
#include <iostream>

namespace VTX::UI
{
	MainWindow::MainWindow( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		_registerEvent( Event::Global::CHANGE_STATE );

		const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
		resize( winsize );

		if ( Setting::Setting::WINDOW_FULLSCREEN_DEFAULT )
			setWindowMode( WindowMode::Fullscreen );
		else
			setWindowMode( WindowMode::Windowed );
	}

	MainWindow::~MainWindow() { delete _contextualMenu; }

	void MainWindow::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::CHANGE_STATE )
		{
			const Event::VTXEventValue<ID::VTX_ID> & event
				= dynamic_cast<const Event::VTXEventValue<ID::VTX_ID> &>( p_event );

			ID::VTX_ID state = event.value;
		}
	}

	void MainWindow::setupUi()
	{
		_renderWidget = WidgetFactory::get().instantiateWidget<Widget::Render::RenderWidget>( this, "renderWidget" );
		_mainMenuBar  = WidgetFactory::get().instantiateWidget<Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
		setMenuBar( _mainMenuBar );

		_sceneWidget = WidgetFactory::get().instantiateWidget<Widget::Scene::SceneWidget>( this, "sceneWidget" );
		_inspectorWidget
			= WidgetFactory::get().instantiateWidget<Widget::Inspector::InspectorWidget>( this, "inspectorWidget" );
		_consoleWidget
			= WidgetFactory::get().instantiateWidget<Widget::Console::ConsoleWidget>( this, "consoleWidget" );
		_sequenceWidget
			= WidgetFactory::get().instantiateWidget<Widget::Sequence::SequenceWidget>( this, "sequenceWidget" );
		_selectionWidget
			= WidgetFactory::get().instantiateWidget<Widget::Selection::SelectionWidget>( this, "selectionWidget" );
		_settingWidget
			= WidgetFactory::get().instantiateWidget<Widget::Settings::SettingWidget>( this, "settingWidget" );

		_statusBarWidget
			= WidgetFactory::get().instantiateWidget<Widget::StatusBar::StatusBarWidget>( this, "statusBar" );
		_statusBarWidget->setFixedHeight( 25 );
		setStatusBar( _statusBarWidget );

		_contextualMenu = new ContextualMenu();

		_setupSlots();

		_setupDock();

		_mainMenuBar->setCurrentTab( 0 );
		_renderWidget->setFocus();

		_loadStyleSheet( VTX_SETTING().STYLESHEET_FILE_DEFAULT );
	}

	void MainWindow::_loadStyleSheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		QString stylesheet = stylesheetFile.readAll();
		setStyleSheet( stylesheet );
	}

	void MainWindow::_setupSlots()
	{
		connect( _sequenceWidget,
				 &Widget::Sequence::SequenceWidget::visibilityChanged,
				 this,
				 &MainWindow::_onDockWindowVisibilityChange );
	}

	void MainWindow::_setupDock()
	{
		setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

		addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _sceneWidget, Qt::Orientation::Horizontal );
		splitDockWidget( _sceneWidget, _selectionWidget, Qt::Orientation::Vertical );
		addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _sequenceWidget, Qt::Orientation::Horizontal );
		splitDockWidget( _sequenceWidget, _renderWidget, Qt::Orientation::Vertical );
		addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _inspectorWidget, Qt::Orientation::Horizontal );
		addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, _consoleWidget, Qt::Orientation::Vertical );

		//_sequenceWidget->hide();
		//_selectionWidget->hide();

		// Create an emplacement for the widget before setting it floating to prevent warning
		// TODO check https://bugreports.qt.io/browse/QTBUG-88157 to remove useless tabifyDockWidget
		tabifyDockWidget( _inspectorWidget, _settingWidget );
		_settingWidget->setFloating( true );
		_settingWidget->hide();
	}

	void MainWindow::_onDockWindowVisibilityChange( const bool p_visible )
	{
		VTX_EVENT( new Event::VTXEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE ) );
	}

	void MainWindow::resizeEvent( QResizeEvent * p_event )
	{
		QMainWindow::resizeEvent( p_event );

		if ( p_event->type() == QEvent::Type::WindowStateChange )
		{
			WindowMode newMode = _getWindowModeFromWindowState( windowState() );
			VTX_EVENT( new Event::VTXEvent( Event::Global::MAIN_WINDOW_MODE_CHANGE ) );
		}
	}

	void MainWindow::closeEvent( QCloseEvent * p_event )
	{
		VTXApp::get().stop();
		p_event->accept();
	}

	void MainWindow::_toggleWidget( QWidget * widget )
	{
		if ( widget->isVisible() )
			widget->hide();
		else
			widget->show();
	}

	void MainWindow::toggleSequenceWindow() const
	{
		if ( _sequenceWidget->isVisible() )
			_sequenceWidget->hide();
		else
			_sequenceWidget->show();
	}

	WindowMode MainWindow::getWindowMode() { return _getWindowModeFromWindowState( windowState() ); }
	void	   MainWindow::setWindowMode( const WindowMode & p_mode )
	{
		const Qt::WindowState state = Qt::WindowState( int( p_mode ) );
		setWindowState( state );

		VTX_EVENT( new Event::VTXEventValue( Event::Global::MAIN_WINDOW_MODE_CHANGE, p_mode ) );
	}
	void MainWindow::toggleWindowState()
	{
		WindowMode mode = _getWindowModeFromWindowState( windowState() );
		if ( mode == WindowMode::Fullscreen )
			setWindowMode( WindowMode::Windowed );
		else
			setWindowMode( WindowMode::Fullscreen );
	}

	bool MainWindow::getWidgetVisibility( const ID::VTX_ID & p_winId ) const
	{
		return getWidget( p_winId ).isVisible();
	};

	void MainWindow::openSettingWindow() const { _settingWidget->show(); }
	void MainWindow::openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const
	{
		_settingWidget->setCurrentMenu( p_menuIndex );
		_settingWidget->show();
	}

	WindowMode MainWindow::_getWindowModeFromWindowState( const Qt::WindowStates & p_state )
	{
		WindowMode res;

		if ( p_state & Qt::WindowState::WindowFullScreen )
			res = WindowMode::Fullscreen;
		else if ( p_state & Qt::WindowState::WindowMaximized )
			res = WindowMode::Maximized;
		else if ( p_state & Qt::WindowState::WindowMinimized )
			res = WindowMode::Minimized;
		else
			res = WindowMode::Windowed;

		return res;
	}

} // namespace VTX::UI
