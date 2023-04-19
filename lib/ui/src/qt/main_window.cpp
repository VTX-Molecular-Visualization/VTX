#include "ui/qt/main_window.hpp"
#include "ui/old_ui/action/main.hpp"
#include "ui/old_ui/controller/base_keyboard_controller.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/qt/action/main.hpp"
#include "ui/qt/action/selection.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/tool/session/dialog.hpp"
#include "ui/qt/util.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/dev.hpp>
#include <app/action/main.hpp>
#include <app/action/selection.hpp>
#include <app/action/setting.hpp>
#include <app/event/vtx_event.hpp>
#include <app/core/event/event_manager.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <app/old_app/io/struct/scene_path_data.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <app/old_app/setting.hpp>
#include <app/old_app/worker/worker_manager.hpp>

namespace VTX::UI::QT
{
	MainWindow::MainWindow( QWidget * p_parent ) : BaseMainWindow(), BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::Event::Global::CHANGE_STATE );
		_registerEvent( VTX::Event::Global::SCENE_MODIFICATION_STATE_CHANGE );
		_registerEvent( VTX::Event::Global::SCENE_PATH_CHANGE );

		_registerEvent( VTX::Event::Global::PICKER_MODE_CHANGE );
	}

	MainWindow::~MainWindow() {}

	void MainWindow::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::CHANGE_STATE )
		{
			const VTX::Event::VTXEventValue<ID::VTX_ID> & event
				= dynamic_cast<const VTX::Event::VTXEventValue<ID::VTX_ID> &>( p_event );

			ID::VTX_ID state = event.value;
		}
		else if ( p_event.name == VTX::Event::Global::SCENE_PATH_CHANGE
				  || p_event.name == VTX::Event::Global::SCENE_MODIFICATION_STATE_CHANGE )
		{
			refreshWindowTitle();
		}
		else if ( p_event.name == VTX::Event::Global::PICKER_MODE_CHANGE )
		{
			_updatePicker();
		}
		// else if ( p_event.name == VTX::Event::Global::RMSD_COMPUTED )
		//{
		//	const VTX::Event::VTXEventRef<const VTX::Tool::Analysis::RMSD::RMSDData> & castedEvent
		//		= dynamic_cast<const VTX::Event::VTXEventRef<const VTX::Tool::Analysis::RMSD::RMSDData> &>( p_event );

		//	const std::string log = VTX::Tool::Analysis::RMSD::getLogString( castedEvent.ref );

		//	VTX_INFO( log );
		//}
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
					{
						QT_APP()->quit();
					}
				} );

			Tool::Session::Dialog::leavingSessionDialog( callback );
		}
		else
		{
			p_closeEvent->accept();
		}
	}

	void MainWindow::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		const QSize winsize = QSize( VTX::Style::WINDOW_WIDTH_DEFAULT, VTX::Style::WINDOW_HEIGHT_DEFAULT );
		resize( winsize );
		setWindowState( Qt::WindowState::WindowNoState );
		refreshWindowTitle();
		setContextMenuPolicy( Qt::ContextMenuPolicy::PreventContextMenu );

		_mainMenuBar = WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
		setMenuBar( _mainMenuBar );
		setAcceptDrops( true );

		//_inspectorWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::Inspector::InspectorWidget>( this, "inspectorWidget" );
		//_consoleWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::Console::ConsoleWidget>( this, "consoleWidget" );
		//_sequenceWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::Sequence::SequenceWidget>( this, "sequenceWidget" );
		//_settingWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::Settings::SettingWidget>( this, "settingWidget" );

		//_informationWidget = WidgetFactory::get().instantiateWidget<Widget::Information::InformationWidget>(
		//	this, "informationWidget" );

		//_structuralAlignmentWidget
		//	=
		// WidgetFactory::get().instantiateWidget<Widget::Analysis::StructuralAlignment::StructuralAlignmentWidget>(
		//	/ this, "structuralAlignmentWidget" );

		// renderWidget->displayOverlay( Widget::Render::Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS,
		//							  Widget::Render::Overlay::OVERLAY_ANCHOR::BOTTOM_CENTER );

		// QWidget * const		centralWidget = new QWidget( this );
		// QVBoxLayout * const layout		  = new QVBoxLayout( centralWidget );
		// layout->addWidget( renderWidget );
		// setCentralWidget( centralWidget );

		//_statusBarWidget
		//	= WidgetFactory::get().instantiateWidget<Widget::StatusBar::StatusBarWidget>( this, "statusBar" );
		//_statusBarWidget->setFixedHeight( 25 );
		// setStatusBar( _statusBarWidget );

		_contextualMenu = new ContextualMenu();
		_cursorHandler	= new CursorHandler();

		setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

		_loadStyleSheet( IO::Filesystem::STYLESHEET_FILE_DEFAULT.string().c_str() );
	}

	void MainWindow::initWindowLayout()
	{
		if ( hasValidLayoutSave() )
			loadLastLayout();
		else
			restoreDefaultLayout();

		if ( VTX_SETTING().getWindowFullscreen() )
			setWindowMode( Core::WindowMode::Fullscreen );
		else
			setWindowMode( Core::WindowMode::Windowed );
	}

	QT::Widget::MainMenu::MenuTooltabWidget & MainWindow::getMainMenuToolTab( const Core::ToolLayoutData & layoutData )
	{
		return dynamic_cast<QT::Widget::MainMenu::MenuTooltabWidget &>( getMainMenu().getTab( layoutData.tabName ) );
	}
	QT::Widget::MainMenu::MenuToolBlockWidget & MainWindow::getMainMenuToolBlock(
		const Core::ToolLayoutData & layoutData )
	{
		return dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
			getMainMenu().getTab( layoutData.tabName ).getToolBlock( layoutData.blockName ) );
	}

	void MainWindow::addShortcut( const std::string & p_shortcut, QAction * const p_action )
	{
		assert( _shortcuts.find( p_shortcut ) == _shortcuts.end() );

		p_action->setParent( this );

		connect( new QShortcut( QKeySequence( tr( p_shortcut.c_str() ) ), this ),
				 &QShortcut::activated,
				 p_action,
				 &QAction::trigger );

		_shortcuts.emplace( p_shortcut );
	}

	void MainWindow::_loadStyleSheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		const QString stylesheet = stylesheetFile.readAll();
		setStyleSheet( stylesheet );
	}

	void MainWindow::appendStylesheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		const QString stylesheetContent = stylesheetFile.readAll();
		const QString newStylesheet		= styleSheet() + '\n' + stylesheetContent;

		setStyleSheet( newStylesheet );
	}

	void MainWindow::_setupSlots()
	{
		// connect( _inspectorWidget, &QDockWidget::visibilityChanged, this,
		//&MainWindow::_onDockWindowVisibilityChange
		// );
		//  !V0.1
		//  connect( _selectionWidget, &QDockWidget::visibilityChanged, this,
		//&MainWindow::_onDockWindowVisibilityChange
		//  );
		// connect( _consoleWidget, &QDockWidget::visibilityChanged, this,
		//&MainWindow::_onDockWindowVisibilityChange );
		// connect( _settingWidget, &QDockWidget::visibilityChanged, this,
		//&MainWindow::_onDockWindowVisibilityChange );
		// connect( _sequenceWidget, &QDockWidget::visibilityChanged, this,
		//&MainWindow::_onDockWindowVisibilityChange
		// );

		// connect( _structuralAlignmentWidget,
		//		 &QDockWidget::visibilityChanged,
		//		 this,
		//		 &MainWindow::_onDockWindowVisibilityChange );

		// Shortcuts.
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
	void MainWindow::localize() { setWindowTitle( "VTX" ); }

	QT::Tool::Render::Widget::RenderWidget * MainWindow::getRender()
	{
		return getPanel<QT::Tool::Render::Widget::RenderWidget>( Tool::RENDER_WINDOW_KEY );
	}
	const QT::Tool::Render::Widget::RenderWidget * const MainWindow::getRender() const
	{
		return getPanel<QT::Tool::Render::Widget::RenderWidget>( Tool::RENDER_WINDOW_KEY );
	}
	bool MainWindow::isOpenGLValid() const { return getRender()->isOpenGLValid(); }
	void MainWindow::updateRender() const { getRender()->updateRender(); }

	void MainWindow::_onShortcutFullscreen() const
	{
		if ( windowState() & Qt::WindowStates::enum_type::WindowFullScreen )
		{
			// VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Windowed ) );
		}
		else
		{
			// VTX_ACTION( new Action::Setting::WindowMode( Core::WindowMode::Fullscreen ) );
		}
	}
	void MainWindow::_onShortcutClearSelection() const
	{
		if ( !Selection::SelectionManager::get().getSelectionModel().isEmpty() )
		{
			VTX_ACTION( new VTX::Action::Selection::ClearSelection(
				VTX::Selection::SelectionManager::get().getSelectionModel() ) );
		}
	}
	void MainWindow::_onShortcutRestoreLayout() const { VTX_ACTION( new QT::Action::Main::RestoreLayout() ); }
	void MainWindow::_onShortcutCompileShaders() const { VTX_ACTION( new VTX::Action::Dev::CompileShaders() ); }
	void MainWindow::_onShortcutActiveRenderer() const
	{
		const bool rendererIsActive = VTX_SETTING().getActivateRenderer();
		VTX_ACTION( new VTX::Action::Setting::ActiveRenderer( !rendererIsActive ) );
	}
	void MainWindow::_onShortcutDelete() const
	{
		if ( Selection::SelectionManager::get().getSelectionModel().isEmpty() == false )
		{
			VTX_ACTION( new VTX::Action::Selection::Delete( Selection::SelectionManager::get().getSelectionModel() ) );
		}
	}
	void MainWindow::_onShortcutOrient() const
	{
		const Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new VTX::UI::QT::Action::Selection::Orient( selection ) );
	}
	void MainWindow::_onShortcutSelectAll() const { VTX_ACTION( new VTX::Action::Selection::SelectAll() ); }
	void MainWindow::_onShortcutCopy() const
	{
		Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
		if ( selectionModel.hasMolecule() )
			VTX_ACTION( new VTX::Action::Selection::Copy( selectionModel ) );
	}
	void MainWindow::_onShortcutExtract() const
	{
		Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
		if ( selectionModel.hasMolecule() )
			VTX_ACTION( new VTX::Action::Selection::Extract( selectionModel ) );
	}
	void MainWindow::_onShortcutSetSelectionPicker() const
	{
		VTX_ACTION( new VTX::Action::Main::ChangePicker( ID::Controller::PICKER ) );
	}
	void MainWindow::_onShortcutSetMeasurementPicker() const
	{
		VTX_ACTION( new VTX::Action::Main::ChangePicker( ID::Controller::MEASUREMENT ) );
	}

	void		MainWindow::refreshWindowTitle() { setWindowTitle( QString::fromStdString( _getWindowTitle() ) ); }
	std::string MainWindow::_getWindowTitle() const
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
		const FilePath & currentSessionFilepath = VTXApp::get().getScenePathData().getCurrentPath();

		if ( !currentSessionFilepath.empty() )
		{
			title += " - " + currentSessionFilepath.filename().string();

			if ( VTXApp::get().getScenePathData().sceneHasModifications() )
			{
				title += Style::WINDOW_TITLE_SCENE_MODIFIED_FEEDBACK;
			}
		}

		return title;
	}

	void MainWindow::restoreDefaultLayout()
	{
		//_restoreDockWidget( _sceneWidget );
		//// !V0.1
		////_restoreDockWidget( _selectionWidget );
		//_restoreDockWidget( _sequenceWidget );
		//_restoreDockWidget( _inspectorWidget );
		//_restoreDockWidget( _consoleWidget );
		//_restoreDockWidget( _settingWidget );
		//_restoreDockWidget( _structuralAlignmentWidget );
		for ( const std::pair<Core::WidgetKey, Core::BasePanel *> & panel : _getPanelMap() )
		{
			QtPanel * const qtPanel = static_cast<QtPanel *>( panel.second );

			if ( qtPanel->getPanelType() == QtPanel::PANEL_TYPE::DOCK_WIDGET )
			{
				_restoreDockWidget( dynamic_cast<QtDockablePanel *>( qtPanel ) );
			}
		}

		for ( const std::pair<Core::WidgetKey, Core::BasePanel *> & panel : _getPanelMap() )
		{
			QtPanel * const qtPanel = static_cast<QtPanel *>( panel.second );

			if ( qtPanel->getPanelType() == QtPanel::PANEL_TYPE::DOCK_WIDGET )
			{
				QtDockablePanel * const dockablePanel = dynamic_cast<QtDockablePanel *>( qtPanel );

				if ( dockablePanel->floatingByDefault )
				{
					addDockWidgetAsFloating(
						dockablePanel, dockablePanel->defaultSize, dockablePanel->visibleByDefault );
				}
				else
				{
					addDockWidgetAsTabified( dockablePanel,
											 dockablePanel->defaultWidgetArea,
											 dockablePanel->defaultOrientation,
											 qtPanel->visibleByDefault );
				}
			}
			else if ( qtPanel->getPanelType() == QtPanel::PANEL_TYPE::FLOATING_WINDOW )
			{
				QtFloatingWindowPanel * const windowPanel = dynamic_cast<QtFloatingWindowPanel *>( qtPanel );
				addFloatingWindow( windowPanel, windowPanel->defaultSize, windowPanel->visibleByDefault );
			}
		}

		// addDockWidgetAsTabified( _sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Horizontal);
		// addDockWidgetAsTabified( _sequenceWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal
		// );
		//// !V0.1
		//// addDockWidgetAsTabified( _selectionWidget, _sceneWidget, Qt::Orientation::Vertical, false );
		// addDockWidgetAsTabified( _sequenceWidget, Qt::DockWidgetArea::TopDockWidgetArea,
		// Qt::Orientation::Horizontal
		//); addDockWidgetAsTabified( 	_inspectorWidget, Qt::DockWidgetArea::RightDockWidgetArea,
		//  Qt::Orientation::Horizontal ); addDockWidgetAsTabified( _consoleWidget,
		//  Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Vertical );

		//_addDockWidgetAsFloating( _settingWidget, Style::SETTINGS_PREFERRED_SIZE, false );
		//_addDockWidgetAsFloating( _structuralAlignmentWidget, Style::STRUCTURAL_ALIGNMENT_PREFERRED_SIZE, false );

		// if ( _informationWidget->isVisible() )
		//	_informationWidget->hide();
	}

	void MainWindow::_restoreDockWidget( QDockWidget * const p_dockWidget )
	{
		if ( dockWidgetArea( p_dockWidget ) != Qt::DockWidgetArea::NoDockWidgetArea )
		{
			removeDockWidget( p_dockWidget );
			restoreDockWidget( p_dockWidget );
			p_dockWidget->setFloating( false );
		}
	}

	void MainWindow::addDockWidgetAsTabified( QDockWidget * const	   p_dockWidget,
											  const Qt::DockWidgetArea p_area,
											  const Qt::Orientation	   p_orientation,
											  const bool			   p_visible )
	{
		addDockWidget( p_area, p_dockWidget, p_orientation );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}

	void MainWindow::addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
											  QDockWidget * const p_neighbour,
											  Qt::Orientation	  p_orientation,
											  const bool		  p_visible )
	{
		splitDockWidget( p_neighbour, p_dockWidget, p_orientation );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}
	void MainWindow::addDockWidgetAsFloating( QDockWidget * const p_dockWidget,
											  const QSize &		  p_size,
											  const bool		  p_visible )
	{
		// Create an emplacement for the widget before setting it floating to prevent warning
		// TODO check https://bugreports.qt.io/browse/QTBUG-88157 to remove useless tabifyDockWidget
		// tabifyDockWidget( _inspectorWidget, p_dockWidget );

		p_dockWidget->setFloating( true );
		p_dockWidget->resize( p_size );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}

	void MainWindow::addFloatingWindow( QDialog * const p_window, const QSize & p_size, const bool p_visible )
	{
		p_window->resize( p_size );

		if ( !p_window->isVisible() && p_visible )
			p_window->show();
		else if ( p_window->isVisible() && !p_visible )
			p_window->hide();
	}

	void MainWindow::resizeEvent( QResizeEvent * p_event )
	{
		QMainWindow::resizeEvent( p_event );

		if ( p_event->type() == QEvent::Type::WindowStateChange )
		{
			Core::WindowMode newMode = _getWindowModeFromWindowState( windowState() );
			VTX_EVENT( new VTX::Event::VTXEvent( VTX::Event::Global::MAIN_WINDOW_MODE_CHANGE ) );
		}
	}

	void MainWindow::showEvent( QShowEvent * p_event )
	{
		QMainWindow::showEvent( p_event );

		QT::Tool::Render::Widget::RenderWidget * const renderWidget = getRender();
		if ( renderWidget != nullptr && !renderWidget->isOpenGLValid() )
		{
			renderWidget->show();
			renderWidget->hide();
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
			std::vector<FilePath> _paths  = std::vector<FilePath>();
			const QList<QUrl> &	  urlList = mimeData->urls();

			for ( const QUrl & url : urlList )
			{
				_paths.emplace_back( FilePath( url.toLocalFile().toStdString() ) );
			}

			VTX_ACTION( new VTX::Action::Main::Open( _paths ) );
		}
	}

	QWidget & MainWindow::getWidget( const Core::WidgetKey & p_winId ) const
	{
		QWidget * widget = nullptr;

		// if ( p_winId == ID::UI::Window::RENDER )
		//	widget = _renderWidget;
		// else if ( p_winId == ID::UI::Window::SCENE )
		//	widget = _sceneWidget;
		// else if ( p_winId == ID::UI::Window::INSPECTOR )
		//	widget = _inspectorWidget;
		// else if ( p_winId == ID::UI::Window::CONSOLE )
		//	widget = _consoleWidget;
		// else if ( p_winId == ID::UI::Window::SEQUENCE )
		//	widget = _sequenceWidget;
		//// !V0.1
		//// else if ( p_winId == ID::UI::Window::SELECTION )
		////	widget = _selectionWidget;
		// else if ( p_winId == ID::UI::Window::SETTINGS )
		//	widget = _settingWidget;
		// else if ( p_winId == ID::UI::Window::INFORMATION )
		//	widget = _informationWidget;
		// else if ( p_winId == ID::UI::Window::STRUCTURAL_ALIGNMENT )
		//	widget = _structuralAlignmentWidget;

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

	Core::WindowMode MainWindow::getWindowMode() { return _getWindowModeFromWindowState( windowState() ); }
	void			 MainWindow::setWindowMode( const Core::WindowMode & p_mode )
	{
		const Qt::WindowStates winStateBefore  = windowState();
		const int			   iwinStateBefore = int( winStateBefore );

		switch ( p_mode )
		{
		case Core::WindowMode::Fullscreen: setWindowState( windowState() | Qt::WindowState::WindowFullScreen ); break;
		case Core::WindowMode::Minimized: setWindowState( windowState() | Qt::WindowState::WindowMinimized ); break;
		case Core::WindowMode::Maximized: setWindowState( windowState() | Qt::WindowState::WindowMaximized ); break;
		case Core::WindowMode::Windowed:
			const Qt::WindowStates winState = windowState();
			const Qt::WindowStates mask
				= Qt::WindowState::WindowMinimized | Qt::WindowState::WindowMaximized | Qt::WindowState::WindowActive;
			setWindowState( winState & mask );
			break;
		}

		VTX_EVENT( new VTX::Event::VTXEventValue( VTX::Event::Global::MAIN_WINDOW_MODE_CHANGE, p_mode ) );
	}
	void MainWindow::toggleWindowState()
	{
		Core::WindowMode mode = _getWindowModeFromWindowState( windowState() );
		if ( mode == Core::WindowMode::Fullscreen )
			setWindowMode( Core::WindowMode::Windowed );
		else
			setWindowMode( Core::WindowMode::Fullscreen );
	}

	bool MainWindow::hasValidLayoutSave() const
	{
		QSettings  settings( QString::fromStdString( IO::Filesystem::getConfigIniFile().string() ),
							 QSettings::IniFormat );
		const bool settingsAreValid = settings.status() == QSettings::NoError && settings.allKeys().length() > 0;

		return settingsAreValid && settings.value( "Version" ).toInt() == Style::LAYOUT_VERSION;
	}

	void MainWindow::loadLastLayout()
	{
		QSettings settings( QString::fromStdString( IO::Filesystem::getConfigIniFile().string() ),
							QSettings::IniFormat );
		restoreGeometry( settings.value( "Geometry" ).toByteArray() );

		// Delayed restore state because all widgets grows when restore in maximized (sizes are stored when maximized,
		// but are restore to minimized and then grow when window state restored.
		// Will be usefull when sizes of layouts not grows with window growing.
		_delayRestoreState();
	}

	void MainWindow::_delayRestoreState()
	{
		// Hide all stuff
		//_sceneWidget->hide();
		//_consoleWidget->hide();
		//_inspectorWidget->hide();
		////_selectionWidget->hide();
		//_settingWidget->hide();
		//_structuralAlignmentWidget->hide();

		_restoreStateTimer = new QTimer( this );
		_restoreStateTimer->setSingleShot( true );
		connect( _restoreStateTimer, &QTimer::timeout, this, &MainWindow::_restoreStateDelayedAction );
		_restoreStateTimer->start( 20 );
	}
	void MainWindow::_restoreStateDelayedAction()
	{
		QSettings settings( QString::fromStdString( IO::Filesystem::getConfigIniFile().string() ),
							QSettings::IniFormat );
		restoreState( settings.value( "WindowState" ).toByteArray() );

		_checkUnknownFloatableWindows();

		delete _restoreStateTimer;
		_restoreStateTimer = nullptr;
		show();
	}

	void MainWindow::saveLayout() const
	{
		QSettings settings( QString::fromStdString( IO::Filesystem::getConfigIniFile().string() ),
							QSettings::IniFormat );
		settings.setValue( "Version", Style::LAYOUT_VERSION );

		settings.setValue( "Geometry", saveGeometry() );
		settings.setValue( "WindowState", saveState() );
	}
	void MainWindow::deleteLayoutSaveFile() const
	{
		QSettings settings( QString::fromStdString( IO::Filesystem::getConfigIniFile().string() ),
							QSettings::IniFormat );
		settings.clear();
	}

	bool MainWindow::getWidgetVisibility( const ID::VTX_ID & p_winId ) const
	{
		return getWidget( p_winId ).isVisible();
	};

	// void MainWindow::openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const
	//{
	//	_settingWidget->setCurrentMenu( p_menuIndex );
	//	_settingWidget->show();
	//	_settingWidget->raise();
	// }

	void MainWindow::updateRenderSetting( const Renderer::RENDER_SETTING p_setting )
	{
		//_renderWidget->updateRenderSetting( p_setting );
	}
	// const Vec2i MainWindow::getPickedIds( const uint p_x, const uint p_y )
	//{
	//	 return _renderWidget->getPickedIds( p_x, p_y );
	// }

	Core::WindowMode MainWindow::_getWindowModeFromWindowState( const Qt::WindowStates & p_state )
	{
		Core::WindowMode res;

		if ( p_state & Qt::WindowState::WindowFullScreen )
			res = Core::WindowMode::Fullscreen;
		else if ( p_state & Qt::WindowState::WindowMaximized )
			res = Core::WindowMode::Maximized;
		else if ( p_state & Qt::WindowState::WindowMinimized )
			res = Core::WindowMode::Minimized;
		else
			res = Core::WindowMode::Windowed;

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
		// const QT::State::Visualization * const visualizationState
		//	= QT_APP()->getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		// const ID::VTX_ID & pickerID = visualizationState->getCurrentPickerID();

		// if ( pickerID == ID::Controller::PICKER )
		//{
		//	_cursorHandler->applyCursor(
		//		CursorHandler::Cursor::DEFAULT, &getWidget( ID::UI::Window::RENDER ), "Picker" );
		// }
		// else if ( pickerID == ID::Controller::MEASUREMENT )
		//{
		//	const Controller::MeasurementPicker * const measurementPicker
		//		= visualizationState->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

		//	CursorHandler::Cursor cursor;

		//	switch ( measurementPicker->getCurrentMode() )
		//	{
		//	case Controller::MeasurementPicker::Mode::DISTANCE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DISTANCE;
		//		break;
		//	case Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DISTANCE_TO_CYCLE;
		//		break;
		//	case Controller::MeasurementPicker::Mode::ANGLE: cursor = CursorHandler::Cursor::MEASUREMENT_ANGLE; break;
		//	case Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DIHEDRAL_ANGLE;
		//		break;
		//	default: cursor = CursorHandler::Cursor::DEFAULT; break;
		//	}

		//	_cursorHandler->applyCursor( cursor, &getWidget( ID::UI::Window::RENDER ), "Picker_Measurement" );
		//}
	}

	void MainWindow::_checkUnknownFloatableWindows()
	{
		//_checkUnknownFloatableWindow( _settingWidget, Style::SETTINGS_PREFERRED_SIZE );
		//_checkUnknownFloatableWindow( _structuralAlignmentWidget, Style::STRUCTURAL_ALIGNMENT_PREFERRED_SIZE );
	}
	void MainWindow::_checkUnknownFloatableWindow( QDockWidget * const p_widget, const QSize & p_defaultSize )
	{
		if ( p_widget->widget()->size().height() == QT_UNKNOWN_WIDGET_DEFAULT_LAYOUT_HEIGHT )
		{
			addDockWidgetAsFloating( p_widget, p_defaultSize, p_widget->isVisible() );
		}
	}
} // namespace VTX::UI::QT
