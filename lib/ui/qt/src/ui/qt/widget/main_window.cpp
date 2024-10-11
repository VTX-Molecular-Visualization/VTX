#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/dialog/progress.hpp"
#include "ui/qt/dock_widget/colors.hpp"
#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/dock_widget/render_settings.hpp"
#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/dock_widget/sequence.hpp"
#include "ui/qt/menu/camera.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/menu/help.hpp"
#include "ui/qt/menu/theme.hpp"
#include "ui/qt/menu/view.hpp"
#include "ui/qt/tool_bar/camera.hpp"
#include "ui/qt/tool_bar/file.hpp"
#include "ui/qt/tool_bar/snapshot.hpp"
#include <QApplication>
#include <app/action/application.hpp>

namespace VTX::UI::QT::Widget
{

	MainWindow::MainWindow() : BaseWidget<MainWindow, QMainWindow>( nullptr )
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

		//  Features.
		setTabShape( QTabWidget::Rounded );

		// setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
		// setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
		setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
		setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
	}

	void MainWindow::build()
	{
		VTX_DEBUG( "Build main window" );

		// Main menu.
		createMenu<Menu::File>();
		createMenu<Menu::Camera>();
		// createMenu<Menu::View>();
		createMenu<Menu::Theme>();
		createMenu<Menu::Help>();

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
		_openGLWidget = new OpenGLWidget( this );
		setCentralWidget( _openGLWidget );

		// Dock widgets.
		createDockWidget<DockWidget::Sequence>( Qt::TopDockWidgetArea );

		createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::Representations>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::Colors>( Qt::LeftDockWidgetArea );

		// dwScene->raise();

		createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::RenderSettings>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );

		// dwInspector->raise();

		createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

		// Status bar.
		_statusBar = new StatusBar( this );
		setStatusBar( _statusBar );
	}

	void MainWindow::prepare()
	{
		// Select default tabs.
		Core::WIDGETS::get().get<DockWidget::Scene>()->raise();
		Core::WIDGETS::get().get<DockWidget::Inspector>()->raise();

		// TODO: Set openGL widget as focus.
		// centralWidget()->setFocus();

		// Backup default geometry and state.
		_defaultGeometry = saveGeometry();
		_defaultState	 = saveState();

		// Connect progress dialog.
		APP::onStartBlockingOperation += [ this ]( const std::string_view p_text )
		{
			// TODO:: don't delete and recreate.
			_progressDialog = new Dialog::Progress( p_text );
			_progressDialog->show();
			// Why?
			QCoreApplication::processEvents();
		};
		APP::onUpdateBlockingOperation += [ this ]( const float p_value ) { _progressDialog->setValue( p_value ); };
		APP::onEndBlockingOperation += [ this ]()
		{
			if ( _progressDialog )
			{
				_progressDialog->close();
				delete _progressDialog;
				_progressDialog = nullptr;
			}
		};
	}

	void MainWindow::addMenuAction( const App::UI::WidgetId & p_menu, const App::UI::DescAction & p_action )
	{
		for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
		{
			if ( menu->title().toStdString() == p_menu )
			{
				menu->addAction( Action::Factory::get( p_action ) );
				return;
			}
		}

		QMenu * const menu = menuBar()->addMenu( p_menu.c_str() );
		menu->addAction( Action::Factory::get( p_action ) );
	}

	void MainWindow::addToolBarAction( const App::UI::WidgetId & p_toolbar, const App::UI::DescAction & p_action )
	{
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolbar->windowTitle().toStdString() == p_toolbar )
			{
				toolbar->addAction( Action::Factory::get( p_action ) );
				return;
			}
		}

		QToolBar * const toolbar = new QToolBar( p_toolbar.c_str(), this );
		addToolBar( toolbar );
		toolbar->addAction( Action::Factory::get( p_action ) );
	}

	void MainWindow::resetLayout()
	{
		// Restore geometry and state.
		restoreGeometry( _defaultGeometry );
		restoreState( _defaultState );
		center();
	}

	void MainWindow::changeEvent( QEvent * const p_event ) {}

	void MainWindow::closeEvent( QCloseEvent * p_event )
	{
		VTX_TRACE( "Qt main window close event" );

		if ( not _closing )
		{
			App::ACTION_SYSTEM().execute<App::Action::Application::Quit>();
			p_event->ignore();
		}
	}

	void MainWindow::save()
	{
		SETTINGS.setValue( "geometry", saveGeometry() );
		SETTINGS.setValue( "windowState", saveState() );
	}

	void MainWindow::restore()
	{
		restoreGeometry( SETTINGS.value( "geometry" ).toByteArray() );
		restoreState( SETTINGS.value( "windowState" ).toByteArray() );
	}

} // namespace VTX::UI::QT::Widget
