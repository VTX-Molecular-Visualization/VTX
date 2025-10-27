#include "ui/qt/widget/main_window.hpp"
#include "app/services.hpp"
#include "ui/qt/dialog/progress.hpp"
#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/dock_widget/render_settings.hpp"
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
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QMimeData>
#include <util/event_hub.hpp>

namespace
{
	constexpr std::string_view _SETTING_KEY_GEOMETRY = "main/geometry";
	constexpr std::string_view _SETTING_KEY_STATE	 = "main/state";
} // namespace

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
		createDockWidget<DockWidget::Sequences>( Qt::TopDockWidgetArea );

		auto * const dwScene = createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::Representations>( Qt::LeftDockWidgetArea );
		createDockWidget<DockWidget::ColorLayouts>( Qt::LeftDockWidgetArea );
		dwScene->raise();

		auto * const dwInspector = createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::RenderSettings>( Qt::RightDockWidgetArea );
		createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
		dwInspector->raise();

		createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

		// Status bar.
		_statusBar = new StatusBar( this );
		setStatusBar( _statusBar );

		centralWidget()->setFocus();

		// Backup default geometry and state.
		_defaultGeometry = saveGeometry();
		_defaultState	 = saveState();

		// Restore geometry and state.
		restoreGeometry( SETTINGS().value( _SETTING_KEY_GEOMETRY ).toByteArray() );
		restoreState( SETTINGS().value( _SETTING_KEY_STATE ).toByteArray() );

		// Connect events.
		App::HUB().connect<App::Events::BlockingOperationStart, &MainWindow::_onBlockingOperationStart>( this );
		App::HUB().connect<App::Events::BlockingOperationProgress, &MainWindow::_onBlockingOperationProgress>( this );
		App::HUB().connect<App::Events::BlockingOperationEnd, &MainWindow::_onBlockingOperationEnd>( this );
	}

	MainWindow::~MainWindow()
	{
		SETTINGS().setValue( _SETTING_KEY_GEOMETRY, saveGeometry() );
		SETTINGS().setValue( _SETTING_KEY_STATE, saveState() );
	}

	void MainWindow::addMenuAction( const App::UI::WidgetId & p_menu, const App::UI::DescAction & p_action )
	{
		for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
		{
			if ( menu->title().toStdString() == p_menu )
			{
				menu->addAction( getAction( p_action ) );
				return;
			}
		}

		QMenu * const menu = menuBar()->addMenu( p_menu.c_str() );
		menu->addAction( getAction( p_action ) );
	}

	void MainWindow::addToolBarAction( const App::UI::WidgetId & p_toolbar, const App::UI::DescAction & p_action )
	{
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolbar->windowTitle().toStdString() == p_toolbar )
			{
				toolbar->addAction( getAction( p_action ) );
				return;
			}
		}

		QToolBar * const toolbar = new QToolBar( p_toolbar.c_str(), this );
		addToolBar( toolbar );
		toolbar->addAction( getAction( p_action ) );
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
			// App::ACTION().execute<App::Action::Scene::LoadSystem>( url.toLocalFile().toStdString() );
		}

		p_event->acceptProposedAction();
	}

	QAction * const MainWindow::_getOrCreateAction( const App::UI::DescAction & p_action )
	{
		// Find existing action.
		QAction * qAction = findChild<QAction *>( p_action.name );

		if ( qAction )
		{
			return qAction;
		}
		else
		{
			qAction = new QAction( this );
			qAction->setObjectName( p_action.name );

			VTX_TRACE( "UI action created: {}", p_action.name );

			// Name.
			qAction->setText( p_action.name.c_str() );
			// Group.
			if ( p_action.group.has_value() )
			{
				auto * qActionGroup = findChild<QActionGroup *>( p_action.group.value() );
				if ( not qActionGroup )
				{
					qActionGroup = new QActionGroup( this );
				}

				qAction->setCheckable( true );
				qActionGroup->addAction( qAction );
			}
			// Tip.
			if ( p_action.tip.has_value() )
			{
				QString tip = p_action.tip.value().c_str();

				if ( p_action.shortcut.has_value() )
				{
					tip.append( " (" + p_action.shortcut.value() + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( p_action.icon.has_value() )
			{
				if ( std::holds_alternative<int>( p_action.icon.value() ) )
				{
					qAction->setIcon(
						QApplication::style()->standardIcon(
							static_cast<QStyle::StandardPixmap>( std::get<int>( p_action.icon.value() ) )
						)
					);
				}
				else if ( std::holds_alternative<std::string>( p_action.icon.value() ) )
				{
					qAction->setIcon( QIcon( ( ":/" + std::get<std::string>( p_action.icon.value() ) ).c_str() ) );
				}
				else
				{
					VTX_ERROR( "Invalid icon type for action: {}", p_action.name );
				}
			}
			// Shortcut.
			if ( p_action.shortcut.has_value() )
			{
				qAction->setShortcut( QKeySequence( p_action.shortcut.value().c_str() ) );
			}
			// Action.
			if ( p_action.trigger.has_value() )
			{
				QObject::connect( qAction, &QAction::triggered, p_action.trigger.value() );
			}
			// Connect.
			// TODO: maybe this is dirty (calling this function to get previously created qAction).
			p_action.connect();
		}

		return qAction;
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
