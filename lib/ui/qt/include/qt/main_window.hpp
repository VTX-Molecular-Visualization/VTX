#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "base_widget.hpp"
#include "dock_widget/console.hpp"
#include "dock_widget/inspector.hpp"
#include "dock_widget/options.hpp"
#include "dock_widget/render_settings.hpp"
#include "dock_widget/representations.hpp"
#include "dock_widget/scene.hpp"
#include "dock_widget/sequence.hpp"
#include "helper.hpp"
#include "menu/camera.hpp"
#include "menu/file.hpp"
#include "menu/help.hpp"
#include "menu/theme.hpp"
#include "menu/view.hpp"
#include "opengl_widget.hpp"
#include "status_bar.hpp"
#include "tool_bar/camera.hpp"
#include "tool_bar/file.hpp"
#include "tool_bar/snapshot.hpp"
#include <QDialog>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
#include <QScreen>
#include <ui/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	class MainWindow : public BaseWidget<MainWindow, QMainWindow>
	{
	  public:
		MainWindow() : BaseWidget<MainWindow, QMainWindow>( nullptr )
		{
			// Size.
			resize( 1920, 1080 );

			setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		}

		virtual ~MainWindow() {}

		void build()
		{
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
			//_openGLWidget = new OpenGLWidget( this );
			// setCentralWidget( _openGLWidget );
			auto * centerPanel = new QDockWidget( "Renderer" );
			setCentralWidget( centerPanel );

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

			// Demo.
			/*
			addMenuAction( MenuAction { "Tool 1", "Action 1" } );
			addMenuAction( MenuAction { "Tool 1", "Action 2" } );

			addToolBarAction( ToolBarAction { "Tool 1", "Action 1" } );
			addToolBarAction( ToolBarAction { "Tool 1", "Action 2" } );
			*/
		}

		/*
		void addMenuAction( const MenuAction & p_ma )
		{
			for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
			{
				if ( menu->title().toStdString() == p_ma.idMenu )
				{
					Helper::addQAction( menu, p_ma.action );
					return;
				}
			}

			QMenu * const menu = menuBar()->addMenu( p_ma.idMenu.c_str() );
			Helper::addQAction( menu, p_ma.action );
		}

		void addToolBarAction( const ToolBarAction & p_tba )
		{
			for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
			{
				if ( toolbar->windowTitle().toStdString() == p_tba.idToolBar )
				{
					Helper::addQAction( toolbar, p_tba.action );
					return;
				}
			}

			QToolBar * const toolbar = new QToolBar( p_tba.idToolBar.c_str(), this );
			addToolBar( toolbar );
			Helper::addQAction( toolbar, p_tba.action );
		}
		*/

		template<typename M>
		M * createMenu()
		{
			M * const menu = new M( menuBar() );
			menuBar()->addMenu( menu );
			return menu;
		}

		template<typename TB>
		TB * createToolBar()
		{
			TB * const toolBar = new TB( this );
			addToolBar( toolBar );
			return toolBar;
		}

		template<typename DW>
		DW * createDockWidget( const Qt::DockWidgetArea & p_area )
		{
			DW * const dockWidget = new DW( this );
			addDockWidget( p_area, dockWidget );
			return dockWidget;
		}

		void resetLayout()
		{
			// Restore geometry and state.
			restoreGeometry( _defaultGeometry );
			restoreState( _defaultState );
			center();
		}

	  private:
		QPointer<OpenGLWidget> _openGLWidget;
		QPointer<StatusBar>	   _statusBar;

		// TODO: keep like that or re-tabify?
		QByteArray _defaultGeometry;
		QByteArray _defaultState;
	};

} // namespace VTX::UI::QT

#endif
