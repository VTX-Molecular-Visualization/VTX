#include "main_window.hpp"
#include "action/dev.hpp"
#include "action/main.hpp"
#include "action/selection.hpp"
#include "controller/base_keyboard_controller.hpp"
#include "controller/measurement_picker.hpp"
#include "event/event_manager.hpp"
#include "io/struct/scene_path_data.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "widget_factory.hpp"
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <QShortcut>
#include <QSize>
#include <iostream>

namespace VTX::UI
{
	MainWindow::MainWindow( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		_registerEvent( Event::Global::CHANGE_STATE );
		_registerEvent( Event::Global::SCENE_MODIFICATION_STATE_CHANGE );
		_registerEvent( Event::Global::SCENE_PATH_CHANGE );

		_registerEvent( Event::Global::PICKER_MODE_CHANGE );
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
		else if ( p_event.name == Event::Global::SCENE_PATH_CHANGE
				  || p_event.name == Event::Global::SCENE_MODIFICATION_STATE_CHANGE )
		{
			refreshWindowTitle();
		}
		else if ( p_event.name == Event::Global::PICKER_MODE_CHANGE )
		{
			_updatePicker();
		}
	}

	void MainWindow::closeEvent( QCloseEvent * p_closeEvent )
	{
		if ( VTXApp::get().hasAnyModifications() )
		{
			p_closeEvent->ignore();
			Worker::CallbackThread callback = Worker::CallbackThread(
				[]( const uint p_code )
				{
					if ( p_code )
						VTXApp::get().quit();
				} );

			UI::Dialog::leavingSessionDialog( callback );
		}
		else
		{
			p_closeEvent->accept();
		}
	}

	void MainWindow::setupUi()
	{
		const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
		resize( winsize );
		setWindowState( Qt::WindowState::WindowNoState );
		refreshWindowTitle();
		setContextMenuPolicy( Qt::ContextMenuPolicy::PreventContextMenu );

		_mainMenuBar = WidgetFactory::get().instantiateWidget<Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
		setMenuBar( _mainMenuBar );
		setAcceptDrops( true );

		_sceneWidget  = WidgetFactory::get().instantiateWidget<Widget::Scene::SceneWidget>( this, "sceneWidget" );
		_renderWidget = WidgetFactory::get().instantiateWidget<Widget::Render::RenderWidget>( this, "renderWidget" );
		_inspectorWidget
			= WidgetFactory::get().instantiateWidget<Widget::Inspector::InspectorWidget>( this, "inspectorWidget" );
		_consoleWidget
			= WidgetFactory::get().instantiateWidget<Widget::Console::ConsoleWidget>( this, "consoleWidget" );
		_sequenceWidget
			= WidgetFactory::get().instantiateWidget<Widget::Sequence::SequenceWidget>( this, "sequenceWidget" );
		// !V0.1
		//_selectionWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::Selection::SelectionWidget>( this, "selectionWidget" );
		_settingWidget
			= WidgetFactory::get().instantiateWidget<Widget::Settings::SettingWidget>( this, "settingWidget" );

		_informationWidget = WidgetFactory::get().instantiateWidget<Widget::Information::InformationWidget>(
			this, "informationWidget" );

		_renderWidget->displayOverlay( Widget::Render::Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS,
									   Widget::Render::Overlay::OVERLAY_ANCHOR::BOTTOM_CENTER );

		QWidget * const		centralWidget = new QWidget( this );
		QVBoxLayout * const layout		  = new QVBoxLayout( centralWidget );
		layout->addWidget( _renderWidget );
		setCentralWidget( centralWidget );

		_statusBarWidget
			= WidgetFactory::get().instantiateWidget<Widget::StatusBar::StatusBarWidget>( this, "statusBar" );
		_statusBarWidget->setFixedHeight( 25 );
		setStatusBar( _statusBarWidget );

		_contextualMenu = new ContextualMenu();
		_cursorHandler	= new CursorHandler();

		_setupSlots();

		setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

		_mainMenuBar->setCurrentTab( 0 );
		_renderWidget->setFocus();

		_loadStyleSheet( Util::Filesystem::STYLESHEET_FILE_DEFAULT.path().c_str() );
	}
	void MainWindow::initWindowLayout()
	{
		if ( hasValidLayoutSave() )
			loadLastLayout();
		else
			restoreDefaultLayout();

		if ( VTX_SETTING().getWindowFullscreen() )
			setWindowMode( WindowMode::Fullscreen );
		else
			setWindowMode( WindowMode::Windowed );
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
		connect( _sceneWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange );
		connect( _inspectorWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange );
		// !V0.1
		// connect( _selectionWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange
		// );
		connect( _consoleWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange );
		connect( _settingWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange );
		connect( _sequenceWidget, &QDockWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange );

		// Shortcuts.
		connect( new QShortcut( QKeySequence( tr( "Ctrl+N" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutNew );
		connect( new QShortcut( QKeySequence( tr( "Ctrl+O" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutOpen );
		connect( new QShortcut( QKeySequence( tr( "Ctrl+S" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutSave );
		connect( new QShortcut( QKeySequence( tr( "Ctrl+Shift+S" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutSaveAs );
		connect( new QShortcut( QKeySequence( tr( "F11" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutFullscreen );
		connect( new QShortcut( QKeySequence( tr( "Esc" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutClearSelection );
		connect( new QShortcut( QKeySequence( tr( "F6" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutRestoreLayout );
#ifndef VTX_PRODUCTION
		connect( new QShortcut( QKeySequence( tr( "F8" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutCompileShaders );
		connect( new QShortcut( QKeySequence( tr( "F9" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutActiveRenderer );
#endif
		connect( new QShortcut( QKeySequence( tr( "Del" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutDelete );
		connect( new QShortcut( QKeySequence( tr( "O" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutOrient );
		connect( new QShortcut( QKeySequence( tr( Controller::BaseKeyboardController::getKeyboardLayout()
														  == Controller::KeyboardLayout::AZERTY
													  ? "Ctrl+A"
													  : "Ctrl+Q" ) ),
								this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutSelectAll );
		connect( new QShortcut( QKeySequence( tr( "Ctrl+D" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutCopy );
		connect( new QShortcut( QKeySequence( tr( "Ctrl+E" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutExtract );

		connect( new QShortcut( QKeySequence( tr( "P" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutSetSelectionPicker );
		connect( new QShortcut( QKeySequence( tr( "M" ) ), this ),
				 &QShortcut::activated,
				 this,
				 &MainWindow::_onShortcutSetMeasurementPicker );
	}

	void MainWindow::_onShortcutNew() { UI::Dialog::createNewSessionDialog(); }

	void MainWindow::_onShortcutOpen() { UI::Dialog::openLoadSessionDialog(); }

	void MainWindow::_onShortcutSave()
	{
		VTX_ACTION_ENQUEUE( new Action::Main::Save( VTXApp::get().getScenePathData().getCurrentPath() ) );
	}

	void MainWindow::_onShortcutSaveAs() { UI::Dialog::openSaveSessionDialog(); }

	void MainWindow::_onShortcutFullscreen()
	{
		if ( windowState() & Qt::WindowStates::enum_type::WindowFullScreen )
		{
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Windowed ) );
		}
		else
		{
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Fullscreen ) );
		}
	}

	void MainWindow::_onShortcutClearSelection()
	{
		if ( !Selection::SelectionManager::get().getSelectionModel().isEmpty() )
		{
			VTX_ACTION(
				new Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
		}
	}

	void MainWindow::_onShortcutRestoreLayout() { VTX_ACTION( new Action::Setting::RestoreLayout() ); }

	void MainWindow::_onShortcutCompileShaders() { VTX_ACTION( new Action::Dev::CompileShaders() ); }

	void MainWindow::_onShortcutActiveRenderer()
	{
		VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().getActivateRenderer() ) );
	}

	void MainWindow::_onShortcutDelete()
	{
		if ( Selection::SelectionManager::get().getSelectionModel().isEmpty() == false )
		{
			VTX_ACTION( new Action::Selection::Delete( Selection::SelectionManager::get().getSelectionModel() ) );
		}
	}

	void MainWindow::_onShortcutOrient()
	{
		const Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();
		if ( selection.hasMolecule() )
		{
			VTX_ACTION( new Action::Selection::Orient( selection ) );
		}
	}

	void MainWindow::_onShortcutSelectAll() { VTX_ACTION( new Action::Selection::SelectAll() ); }

	void MainWindow::_onShortcutCopy()
	{
		Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
		if ( selectionModel.hasMolecule() )
			VTX_ACTION( new Action::Selection::Copy( selectionModel ) );
	}

	void MainWindow::_onShortcutExtract()
	{
		Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
		if ( selectionModel.hasMolecule() )
			VTX_ACTION( new Action::Selection::Extract( selectionModel ) );
	}

	void MainWindow::_onShortcutSetSelectionPicker()
	{
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::PICKER ) );
	}
	void MainWindow::_onShortcutSetMeasurementPicker()
	{
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT,
													int( Controller::MeasurementPicker::Mode::DISTANCE ) ) );
	}

	void MainWindow::refreshWindowTitle()
	{
		std::string title = VTX_PROJECT_NAME + " v" + std::to_string( VTX_VERSION_MAJOR ) + "."
							+ std::to_string( VTX_VERSION_MINOR ) + "." + std::to_string( VTX_VERSION_REVISION )
							+ " (BETA)";
#ifndef VTX_PRODUCTION
		title += " - DEV";
#ifdef _DEBUG
		title += " - DEBUG";
#else
		title += " - RELEASE";
#endif
#endif
		const IO::FilePath & currentSessionFilepath = VTXApp::get().getScenePathData().getCurrentPath();

		if ( !currentSessionFilepath.empty() )
		{
			title += " - " + currentSessionFilepath.filename();

			if ( VTXApp::get().getScenePathData().sceneHasModifications() )
			{
				title += Style::WINDOW_TITLE_SCENE_MODIFIED_FEEDBACK;
			}
		}

		setWindowTitle( QString::fromStdString( title ) );
	}

	void MainWindow::restoreDefaultLayout()
	{
		if ( dockWidgetArea( _sceneWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( _sceneWidget );
			restoreDockWidget( _sceneWidget );
			_sceneWidget->setFloating( false );
		}

		// !V0.1
		// if ( dockWidgetArea( _selectionWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		//{
		//	removeDockWidget( _selectionWidget );
		//	restoreDockWidget( _selectionWidget );
		//	_selectionWidget->setFloating( false );
		//}
		if ( dockWidgetArea( _sequenceWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( _sequenceWidget );
			restoreDockWidget( _sequenceWidget );
			_sequenceWidget->setFloating( false );
		}
		if ( dockWidgetArea( _inspectorWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( _inspectorWidget );
			restoreDockWidget( _inspectorWidget );
			_inspectorWidget->setFloating( false );
		}
		if ( dockWidgetArea( _consoleWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( _consoleWidget );
			restoreDockWidget( _consoleWidget );
			_consoleWidget->setFloating( false );
		}
		if ( dockWidgetArea( _settingWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( _settingWidget );
			restoreDockWidget( _settingWidget );
			_settingWidget->setFloating( false );
		}

		addDockWidget( Qt::DockWidgetArea::LeftDockWidgetArea, _sceneWidget, Qt::Orientation::Horizontal );
		// !V0.1
		// splitDockWidget( _sceneWidget, _selectionWidget, Qt::Orientation::Vertical );
		addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _sequenceWidget, Qt::Orientation::Horizontal );
		addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, _inspectorWidget, Qt::Orientation::Horizontal );
		addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, _consoleWidget, Qt::Orientation::Vertical );

		// Create an emplacement for the widget before setting it floating to prevent warning
		// TODO check https://bugreports.qt.io/browse/QTBUG-88157 to remove useless tabifyDockWidget
		tabifyDockWidget( _inspectorWidget, _settingWidget );
		_settingWidget->setFloating( true );
		_settingWidget->resize( Style::SETTINGS_PREFERRED_SIZE );

		if ( !_sceneWidget->isVisible() )
			_sceneWidget->show();
		// !V0.1
		// if ( !_selectionWidget->isVisible() )
		//	_selectionWidget->show();
		if ( !_sequenceWidget->isVisible() )
			_sequenceWidget->show();
		if ( !_inspectorWidget->isVisible() )
			_inspectorWidget->show();
		if ( !_consoleWidget->isVisible() )
			_consoleWidget->show();

		if ( _settingWidget->isVisible() )
			_settingWidget->hide();

		if ( _informationWidget->isVisible() )
			_informationWidget->hide();
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

	void MainWindow::showEvent( QShowEvent * p_event )
	{
		QMainWindow::showEvent( p_event );

		if ( !_renderWidget->getOpenGLWidget().isValid() )
		{
			_renderWidget->show();
			_renderWidget->hide();
		}
	}

	void MainWindow::dragEnterEvent( QDragEnterEvent * p_event )
	{
		// if ( p_event->mimeData()->hasFormat( "text/plain" ) )
		{
			p_event->acceptProposedAction();
		}
	}

	void MainWindow::dropEvent( QDropEvent * p_event )
	{
		const QMimeData * const mimeData = p_event->mimeData();

		if ( mimeData->hasUrls() )
		{
			std::vector<IO::FilePath> _paths  = std::vector<IO::FilePath>();
			const QList<QUrl> &		  urlList = mimeData->urls();

			for ( const QUrl & url : urlList )
			{
				_paths.emplace_back( IO::FilePath( url.toLocalFile().toStdString() ) );
			}

			VTX_ACTION( new Action::Main::Open( _paths ) );
		}
	}

	QWidget & MainWindow::getWidget( const ID::VTX_ID & p_winId ) const
	{
		QWidget * widget = nullptr;

		if ( p_winId == ID::UI::Window::RENDER )
			widget = _renderWidget;
		else if ( p_winId == ID::UI::Window::SCENE )
			widget = _sceneWidget;
		else if ( p_winId == ID::UI::Window::INSPECTOR )
			widget = _inspectorWidget;
		else if ( p_winId == ID::UI::Window::CONSOLE )
			widget = _consoleWidget;
		else if ( p_winId == ID::UI::Window::SEQUENCE )
			widget = _sequenceWidget;
		// !V0.1
		// else if ( p_winId == ID::UI::Window::SELECTION )
		//	widget = _selectionWidget;
		else if ( p_winId == ID::UI::Window::SETTINGS )
			widget = _settingWidget;
		else if ( p_winId == ID::UI::Window::INFORMATION )
			widget = _informationWidget;

		return *widget;
	}

	void MainWindow::showWidget( const ID::VTX_ID & p_winId, const bool p_show ) const
	{
		QWidget & widget = getWidget( p_winId );

		if ( p_show )
		{
			widget.show();
			widget.raise();
		}
		else
		{
			widget.hide();
		}
	}
	void MainWindow::toggleWidget( const ID::VTX_ID & p_winId ) const
	{
		showWidget( p_winId, !getWidget( p_winId ).isVisible() );
	}

	WindowMode MainWindow::getWindowMode() { return _getWindowModeFromWindowState( windowState() ); }
	void	   MainWindow::setWindowMode( const WindowMode & p_mode )
	{
		const Qt::WindowStates winStateBefore  = windowState();
		const int			   iwinStateBefore = int( winStateBefore );

		switch ( p_mode )
		{
		case WindowMode::Fullscreen: setWindowState( windowState() | Qt::WindowState::WindowFullScreen ); break;
		case WindowMode::Minimized: setWindowState( windowState() | Qt::WindowState::WindowMinimized ); break;
		case WindowMode::Maximized: setWindowState( windowState() | Qt::WindowState::WindowMaximized ); break;
		case WindowMode::Windowed:
			const Qt::WindowStates winState = windowState();
			const Qt::WindowStates mask
				= Qt::WindowState::WindowMinimized | Qt::WindowState::WindowMaximized | Qt::WindowState::WindowActive;
			setWindowState( winState & mask );
			break;
		}

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

	bool MainWindow::hasValidLayoutSave() const
	{
		QSettings  settings( Util::Filesystem::getConfigIniFile().qpath(), QSettings::IniFormat );
		const bool settingsAreValid = settings.status() == QSettings::NoError && settings.allKeys().length() > 0;

		return settingsAreValid && settings.value( "Version" ).toInt() == Style::LAYOUT_VERSION;
	}

	void MainWindow::loadLastLayout()
	{
		QSettings settings( Util::Filesystem::getConfigIniFile().qpath(), QSettings::IniFormat );
		restoreGeometry( settings.value( "Geometry" ).toByteArray() );

		// Delayed restore state because all widgets grows when restore in maximized (sizes are stored when maximized,
		// but are restore to minimized and then grow when window state restored.
		// Will be usefull when sizes of layouts not grows with window growing.
		_delayRestoreState();
	}

	void MainWindow::_delayRestoreState()
	{
		// Hide all stuff
		_sceneWidget->hide();
		_consoleWidget->hide();
		_inspectorWidget->hide();
		//_selectionWidget->hide();
		_settingWidget->hide();

		_restoreStateTimer = new QTimer( this );
		_restoreStateTimer->setSingleShot( true );
		connect( _restoreStateTimer, &QTimer::timeout, this, &MainWindow::_restoreStateDelayedAction );
		_restoreStateTimer->start( 20 );
	}
	void MainWindow::_restoreStateDelayedAction()
	{
		QSettings settings( Util::Filesystem::getConfigIniFile().qpath(), QSettings::IniFormat );
		restoreState( settings.value( "WindowState" ).toByteArray() );
		delete _restoreStateTimer;
		_restoreStateTimer = nullptr;
		show();
	}

	void MainWindow::saveLayout() const
	{
		QSettings settings( Util::Filesystem::getConfigIniFile().qpath(), QSettings::IniFormat );
		settings.setValue( "Version", Style::LAYOUT_VERSION );

		settings.setValue( "Geometry", saveGeometry() );
		settings.setValue( "WindowState", saveState() );
	}
	void MainWindow::deleteLayoutSaveFile() const
	{
		QSettings settings( Util::Filesystem::getConfigIniFile().qpath(), QSettings::IniFormat );
		settings.clear();
	}

	bool MainWindow::getWidgetVisibility( const ID::VTX_ID & p_winId ) const
	{
		return getWidget( p_winId ).isVisible();
	};

	void MainWindow::openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const
	{
		_settingWidget->setCurrentMenu( p_menuIndex );
		_settingWidget->show();
		_settingWidget->raise();
	}

	bool MainWindow::isOpenGLValid() const { return _renderWidget->getOpenGLWidget().isValid(); }

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

	void MainWindow::changeEvent( QEvent * p_event )
	{
		QMainWindow::changeEvent( p_event );
		if ( p_event->type() == QEvent::ActivationChange && !isActiveWindow() )
		{
			Event::EventManager::get().clearKeyboardInputEvents();
		}
	}

	bool MainWindow::eventFilter( QObject * const p_watched, QEvent * const p_event )
	{
		if ( p_event->type() == QEvent::Type::KeyPress )
		{
			keyPressEvent( dynamic_cast<QKeyEvent *>( p_event ) );
			return true;
		}
		else if ( p_event->type() == QEvent::Type::KeyRelease )
		{
			keyReleaseEvent( dynamic_cast<QKeyEvent *>( p_event ) );
			return true;
		}

		return false;
	}

	void MainWindow::_updatePicker() const
	{
		const State::Visualization * const visualizationState
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		const ID::VTX_ID & pickerID = visualizationState->getCurrentPickerID();

		if ( pickerID == ID::Controller::PICKER )
		{
			_cursorHandler->applyCursor(
				CursorHandler::Cursor::DEFAULT, &getWidget( ID::UI::Window::RENDER ), "Picker" );
		}
		else if ( pickerID == ID::Controller::MEASUREMENT )
		{
			const Controller::MeasurementPicker * const measurementPicker
				= visualizationState->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			CursorHandler::Cursor cursor;

			switch ( measurementPicker->getCurrentMode() )
			{
			case Controller::MeasurementPicker::Mode::DISTANCE:
				cursor = CursorHandler::Cursor::MEASUREMENT_DISTANCE;
				break;
			default: cursor = CursorHandler::Cursor::DEFAULT; break;
			}

			_cursorHandler->applyCursor( cursor, &getWidget( ID::UI::Window::RENDER ), "Picker_Measurement" );
		}
	}

} // namespace VTX::UI
