#include "ui/qt/widget/main_window.hpp"
#include "app/helper/io.hpp"
#include "app/services.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/dialog/progress.hpp"
#include "ui/qt/dialog/trajectory_association.hpp"
#include "ui/qt/dialog/updater.hpp"
#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/dock_widget/graphics_configs.hpp"
#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/menu/camera.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/menu/help.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/menu/theme.hpp"
#include "ui/qt/menu/tool.hpp"
#include "ui/qt/menu/view.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/tool_bar/camera.hpp"
#include "ui/qt/tool_bar/file.hpp"
#include "ui/qt/tool_bar/selection.hpp"
#include "ui/qt/tool_bar/snapshot.hpp"
#include <QApplication>
#include <QMessageBox>
#include <QMimeData>
#include <QTimer>
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{

	MainWindow::MainWindow() : BaseWidget( nullptr )
	{
		// Size.
		QRect geometry = screen()->availableGeometry();
		QSize size	   = geometry.size();
		size *= Style::DEFAULT_SIZE_SCALE;
		resize( size );

		Helper::centerWidget( *this, geometry );

		// Set all settings.
		setDockNestingEnabled( false );
		setAnimated( true );
		setUnifiedTitleAndToolBarOnMac( true );
		setDockOptions( AllowTabbedDocks );
		setDockOptions( ForceTabbedDocks );
		setTabPosition( Qt::AllDockWidgetAreas, QTabWidget::North );
		setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		setAcceptDrops( true );

		//  Features.
		setTabShape( QTabWidget::Rounded );

		// Dock layout.
		setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
		setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

		VTX_DEBUG( "Build main window" );

		// Main menu.
		createMenu<Menu::File>();
		createMenu<Menu::Selection>();
		createMenu<Menu::Camera>();
		createMenu<Menu::Tool>();
		createMenu<Menu::View>();
		// createMenu<Menu::Theme>();
		createMenu<Menu::Help>();

		// Toolbars.
		createToolBar<ToolBar::File>();
		createToolBar<ToolBar::Camera>();
		createToolBar<ToolBar::Selection>();
		createToolBar<ToolBar::Snapshot>();

		// Main area : opengl widget.
		auto * renderer = new Renderer( this );
		// renderer->createToolBar<ToolBar::Selection>( Renderer::HUD_POSITION::TOP_RIGHT );
		setCentralWidget( renderer );

		// Dock widgets.
		createDockWidget<DockWidget::Sequences>( Qt::TopDockWidgetArea );

		auto * dwScene = createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::Representations>( Qt::LeftDockWidgetArea )->hide();
		createDockWidget<DockWidget::ColorLayouts>( Qt::LeftDockWidgetArea )->hide();
		dwScene->raise();

		auto * dwInspector = createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::GraphicsConfigs>( Qt::RightDockWidgetArea )->hide();
		auto * dwOptions = createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
		dwInspector->raise();

		auto * console = createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

		// Resize.
		resizeDocks(
			{ dwScene, dwInspector, dwOptions },
			{ Style::DEFAULT_LATERAL_DOCK_WIDTH, Style::DEFAULT_LATERAL_DOCK_WIDTH, Style::DEFAULT_LATERAL_DOCK_WIDTH },
			Qt::Horizontal
		);
		resizeDocks( { console }, { Style::DEFAULT_CONSOLE_HEIGHT }, Qt::Vertical );

		// Status bar.
		_statusBar = new StatusBar( this );
		setStatusBar( _statusBar );

		// Focus central widgetrend to enable shortcuts.
		centralWidget()->setFocus();

		// Backup default geometry and state.
		_defaultGeometry = saveGeometry();
		_defaultState	 = saveState();

		// Restore geometry and state.
		restoreGeometry( SETTINGS().value( SETTING_KEY_GEOMETRY ).toByteArray() );
		restoreState( SETTINGS().value( SETTING_KEY_STATE ).toByteArray() );

		// Connect events.
		App::HUB().connect<App::Events::ApplicationError, &MainWindow::_onApplicationError>( this );
		App::HUB().connect<App::Events::BlockingOperationStart, &MainWindow::_onBlockingOperationStart>( this );
		App::HUB().connect<App::Events::BlockingOperationProgress, &MainWindow::_onBlockingOperationProgress>( this );
		App::HUB().connect<App::Events::BlockingOperationEnd, &MainWindow::_onBlockingOperationEnd>( this );
		App::HUB().connect<App::Events::UpdateAvailable, &MainWindow::_onUpdateAvailable>( this );
		App::HUB().connect<App::Events::RendererResize, &MainWindow::_onRendererResize>( this );
	}

	MainWindow::~MainWindow()
	{
		SETTINGS().setValue( SETTING_KEY_GEOMETRY, saveGeometry() );
		SETTINGS().setValue( SETTING_KEY_STATE, saveState() );
	}

	bool MainWindow::event( QEvent * p_event )
	{
		if ( p_event != nullptr )
		{
			switch ( p_event->type() )
			{
			case QEvent::LayoutRequest:
			case QEvent::UpdateRequest:
			case QEvent::WindowStateChange:
				QTimer::singleShot( 0, this, []() { App::RENDERER().setNeedUpdate( true ); } );
				if ( p_event->type() == QEvent::WindowStateChange )
				{
					UI_ACTIONS().setChecked( Action::View::FULLSCREEN, isFullScreen() );
				}
				break;
			default: break;
			}
		}

		return QMainWindow::event( p_event );
	}

	void MainWindow::addMenuAction( const App::UI::WidgetId & p_menu, const std::string_view p_actionId )
	{
		for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
		{
			if ( menu->title().toStdString() == p_menu )
			{
				UI_ACTIONS().addTo( *menu, p_actionId );
				return;
			}
		}

		QMenu * const menu = menuBar()->addMenu( p_menu.data() );
		UI_ACTIONS().addTo( *menu, p_actionId );
	}

	void MainWindow::addToolBarAction( const App::UI::WidgetId & p_toolbar, const std::string_view p_actionId )
	{
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolbar->windowTitle().toStdString() == p_toolbar )
			{
				UI_ACTIONS().addTo( *toolbar, p_actionId );
				return;
			}
		}

		QToolBar * const toolbar = new QToolBar( p_toolbar.data(), this );
		addToolBar( toolbar );
		UI_ACTIONS().addTo( *toolbar, p_actionId );
	}

	void MainWindow::resetLayout()
	{
		// Restore geometry and state.
		restoreGeometry( _defaultGeometry );
		restoreState( _defaultState );
	}

	uintptr_t MainWindow::getNativeSurface() const
	{
		const auto * widget = static_cast<const Renderer *>( centralWidget() );
		assert( widget != nullptr );
		return widget->getNativeSurface();
	}

	uintptr_t MainWindow::getNativeDisplay() const
	{
		const auto * widget = static_cast<const Renderer *>( centralWidget() );
		assert( widget != nullptr );
		return widget->getNativeDisplay();
	}

	uint8_t MainWindow::getNativePlatform() const
	{
		const auto * widget = static_cast<const Renderer *>( centralWidget() );
		assert( widget != nullptr );
		return widget->getNativePlatform();
	}

	void MainWindow::populateViewMenu( QMenu & p_menu )
	{
		p_menu.clear();

		QAction * const fullscreenAction = UI_ACTIONS().addTo( p_menu, Action::View::FULLSCREEN );
		if ( fullscreenAction != nullptr )
		{
			fullscreenAction->setChecked( isFullScreen() );
		}
		p_menu.addSeparator();

		QAction * const panelsLabel = p_menu.addAction( "Panels" );
		panelsLabel->setEnabled( false );
		for ( QDockWidget * const dock : findChildren<QDockWidget *>() )
		{
			p_menu.addAction( dock->toggleViewAction() );
		}

		p_menu.addSeparator();
		QAction * const toolbarsLabel = p_menu.addAction( "Toolbars" );
		toolbarsLabel->setEnabled( false );
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolBarArea( toolbar ) == Qt::NoToolBarArea )
			{
				continue;
			}
			p_menu.addAction( toolbar->toggleViewAction() );
		}
	}

	QMenu * MainWindow::createPopupMenu()
	{
		QMenu * const menu = new QMenu( this );
		menu->setTitle( "View" );
		populateViewMenu( *menu );
		return menu;
	}

	void MainWindow::closeEvent( QCloseEvent * p_event )
	{
		VTX_TRACE( "MainWindow::closeEvent: Qt main window close event" );
		QCoreApplication::quit();
	}

	void MainWindow::dragEnterEvent( QDragEnterEvent * p_event ) { p_event->acceptProposedAction(); }

	void MainWindow::dropEvent( QDropEvent * p_event )
	{
		for ( const auto & url : p_event->mimeData()->urls() )
		{
			switch ( App::Helper::IO::whatToDoWithThisFile( url.toLocalFile().toStdString() ) )
			{
			case App::Helper::IO::FileDropHandling::actionOpen:
				App::ACTION().execute<App::Action::IO::Open>( url.toLocalFile().toStdString() );
				break;
			case App::Helper::IO::FileDropHandling::associateTrajectory:
			{
				Dialog::TrajectoryAssociation dialog( url.toLocalFile().toStdString() );
				dialog.exec();
				break;
			}
			default: App::ACTION().execute<App::Action::IO::Open>( url.toLocalFile().toStdString() );
			}
		}

		p_event->acceptProposedAction();
	}

	void MainWindow::_onApplicationError( const App::Events::ApplicationError & p_e )
	{
		VTX_ERROR( "{}", p_e.message );
		QMessageBox::critical( nullptr, "Error", p_e.message.c_str(), QMessageBox::StandardButton::Ok );
	}

	void MainWindow::_onBlockingOperationStart( const App::Events::BlockingOperationStart & p_e )
	{
		_progressDialog = new Dialog::Progress( p_e.message );
		_progressDialog->show();
		// Need to process events to display the dialog immediately, because main thread is busy.
		QCoreApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
	}

	void MainWindow::_onBlockingOperationProgress( const App::Events::BlockingOperationProgress & p_e )
	{
		_progressDialog->setValue( p_e.progress );
	}

	void MainWindow::_onBlockingOperationEnd( const App::Events::BlockingOperationEnd & )
	{
		if ( _progressDialog )
		{
			_progressDialog->close();
			delete _progressDialog;
			_progressDialog = nullptr;
		}
	}

	void MainWindow::_onUpdateAvailable( const App::Events::UpdateAvailable & p_e )
	{
		Dialog::Updater * const dialog = new Dialog::Updater( p_e );
		dialog->setAttribute( Qt::WA_DeleteOnClose, true );
		dialog->setModal( true );
		dialog->open();
	}

	void MainWindow::_onRendererResize( const App::Events::RendererResize & p_e )
	{
		if ( not p_e.resizeMainWindow || not isFullScreen() )
		{
			return;
		}

		showNormal();
		UI_ACTIONS().setChecked( Action::View::FULLSCREEN, false );
	}

} // namespace VTX::UI::QT::Widget
