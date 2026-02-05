#include "ui/qt/widget/main_window.hpp"
#include "app/helper/io.hpp"
#include "app/services.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/dialog/progress.hpp"
#include "ui/qt/dialog/trajectory_association.hpp"
#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/dock_widget/graphics_configs.hpp"
#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/menu/camera.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/menu/help.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/menu/theme.hpp"
#include "ui/qt/menu/view.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/tool_bar/camera.hpp"
#include "ui/qt/tool_bar/file.hpp"
#include "ui/qt/tool_bar/snapshot.hpp"
#include <QApplication>
#include <QMessageBox>
#include <QMimeData>
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{

	MainWindow::MainWindow() : BaseWidget( nullptr )
	{
		// Size.
		resize( 1920, 1080 );

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

		// setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
		// setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
		setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
		setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

		VTX_DEBUG( "Build main window" );

		// Main menu.
		createMenu<Menu::File>();
		createMenu<Menu::Camera>();
		createMenu<Menu::Selection>();
		createMenu<Menu::View>()->setEnabled( false );
		createMenu<Menu::Theme>();
		createMenu<Menu::Help>()->setEnabled( false );

		// Toolbars.
		createToolBar<ToolBar::File>();
		createToolBar<ToolBar::Camera>();
		createToolBar<ToolBar::Snapshot>();

		// Add combobox in toobar.
		/*
		QComboBox * comboBox = new QComboBox( this );
		comboBox->addItem( "Item 1" );
		comboBox->addItem( "Item 2" );
		comboBox->addItem( "Item 3" );
		comboBox->addItem( "Item 4" );
		comboBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
		// toolBarArea( Qt::TopToolBarArea ).addWidget( comboBox );
		*/

		// Main area : opengl widget.
		setCentralWidget( new OpenGLWidget( this ) );

		// Dock widgets.
		createDockWidget<DockWidget::Sequences>( Qt::TopDockWidgetArea );

		auto * const dwScene = createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::Representations>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::ColorLayouts>( Qt::LeftDockWidgetArea );
		dwScene->raise();

		auto * const dwInspector = createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::GraphicsConfigs>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
		dwInspector->raise();

		createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

		// Status bar.
		_statusBar = new StatusBar( this );
		setStatusBar( _statusBar );

		// centralWidget()->setFocus();

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

		QTimer::singleShot( 0, this, [ this ] { centralWidget()->setFocus(); } );
	}

	MainWindow::~MainWindow()
	{
		SETTINGS().setValue( SETTING_KEY_GEOMETRY, saveGeometry() );
		SETTINGS().setValue( SETTING_KEY_STATE, saveState() );
	}

	void MainWindow::addMenuAction( const App::UI::WidgetId & p_menu, const App::UI::DescAction & p_action )
	{
		for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
		{
			if ( menu->title().toStdString() == p_menu )
			{
				menu->addAction( Application::getAction( p_action ) );
				return;
			}
		}

		QMenu * const menu = menuBar()->addMenu( p_menu.c_str() );
		menu->addAction( Application::getAction( p_action ) );
	}

	void MainWindow::addToolBarAction( const App::UI::WidgetId & p_toolbar, const App::UI::DescAction & p_action )
	{
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolbar->windowTitle().toStdString() == p_toolbar )
			{
				toolbar->addAction( Application::getAction( p_action ) );
				return;
			}
		}

		QToolBar * const toolbar = new QToolBar( p_toolbar.c_str(), this );
		addToolBar( toolbar );
		toolbar->addAction( Application::getAction( p_action ) );
	}

	void MainWindow::resetLayout()
	{
		// Restore geometry and state.
		restoreGeometry( _defaultGeometry );
		restoreState( _defaultState );
		center();
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
				_trajAssocDialog = new Dialog::TrajectoryAssociation( url.toLocalFile().toStdString() );
				_trajAssocDialog->exec();
				break;
			default: App::ACTION().execute<App::Action::IO::Open>( url.toLocalFile().toStdString() );
			}
		}

		p_event->acceptProposedAction();
	}

	void MainWindow::_onApplicationError( const App::Events::ApplicationError & p_e )
	{
		VTX_ERROR( "{}", p_e.message );
		QMessageBox::critical( this, "Error", p_e.message.c_str(), QMessageBox::StandardButton::Ok );
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

} // namespace VTX::UI::QT::Widget
