#include "qt/main_window.hpp"
#include "qt/dock_widget/console.hpp"
#include "qt/dock_widget/inspector.hpp"
#include "qt/dock_widget/options.hpp"
#include "qt/dock_widget/render_settings.hpp"
#include "qt/dock_widget/representations.hpp"
#include "qt/dock_widget/scene.hpp"
#include "qt/dock_widget/sequence.hpp"
#include "qt/menu/camera.hpp"
#include "qt/menu/file.hpp"
#include "qt/menu/help.hpp"
#include "qt/menu/theme.hpp"
#include "qt/menu/view.hpp"
#include "qt/tool_bar/camera.hpp"
#include "qt/tool_bar/file.hpp"
#include "qt/tool_bar/snapshot.hpp"

namespace VTX::UI::QT
{

	MainWindow::MainWindow() : BaseWidget<MainWindow, QMainWindow>( nullptr )
	{
		// Size.
		resize( 1920, 1080 );

		setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	}

	void MainWindow::build()
	{
		VTX_INFO( "Build main window" );

		// Main menu.
		createMenu<Menu::File>();
		createMenu<Menu::Camera>();
		createMenu<Menu::View>();
		createMenu<Menu::Theme>();
		createMenu<Menu::Help>();

		// Toolbars.
		createToolBar<ToolBar::File>();
		createToolBar<ToolBar::Camera>();
		createToolBar<ToolBar::Snapshot>();

		// Main area : opengl widget.
		_openGLWidget = new OpenGLWidget( this );
		setCentralWidget( _openGLWidget );

		// Dock widgets.
		createDockWidget<DockWidget::Sequence>( Qt::TopDockWidgetArea );

		auto * dwScene			 = createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
		auto * dwRepresentations = createDockWidget<DockWidget::Representations>( Qt::LeftDockWidgetArea );
		tabifyDockWidget( dwScene, dwRepresentations );
		dwScene->raise();

		auto * dwInspector		= createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
		auto * dwRenderSettings = createDockWidget<DockWidget::RenderSettings>( Qt::RightDockWidgetArea );
		auto * dwOptions		= createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
		tabifyDockWidget( dwInspector, dwRenderSettings );
		tabifyDockWidget( dwInspector, dwOptions );
		dwInspector->raise();

		createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

		// Status bar.
		_statusBar = new StatusBar( this );
		setStatusBar( _statusBar );

		// Backup default geometry and state.
		_defaultGeometry = saveGeometry();
		_defaultState	 = saveState();

		// Tool test.
		/*
		UI::Action action3 { "Action 3" };
		addMenuAction( "Tool 1", { "Action 1" } );
		addMenuAction( "Tool 1", { "Action 2" } );
		addMenuAction( "Tool 1", action3 );

		addToolBarAction( "Tool 1", { "Action 1" } );
		addToolBarAction( "Tool 1", { "Action 2" } );
		addToolBarAction( "Tool 1", action3 );
		*/
	}

	void MainWindow::addMenuAction( const WidgetId & p_menu, const UI::Action & p_action )
	{
		for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
		{
			if ( menu->title().toStdString() == p_menu )
			{
				menu->addAction( ACTION( p_action ) );
				return;
			}
		}

		QMenu * const menu = menuBar()->addMenu( p_menu.c_str() );
		menu->addAction( ACTION( p_action ) );
	}

	void MainWindow::addToolBarAction( const WidgetId & p_toolbar, const UI::Action & p_action )
	{
		for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
		{
			if ( toolbar->windowTitle().toStdString() == p_toolbar )
			{
				toolbar->addAction( ACTION( p_action ) );
				return;
			}
		}

		QToolBar * const toolbar = new QToolBar( p_toolbar.c_str(), this );
		addToolBar( toolbar );
		toolbar->addAction( ACTION( p_action ) );
	}

	void MainWindow::resetLayout()
	{
		// Restore geometry and state.
		restoreGeometry( _defaultGeometry );
		restoreState( _defaultState );
		center();
	}

} // namespace VTX::UI::QT
