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
#include "menu/file.hpp"
#include "menu/help.hpp"
#include "menu/view.hpp"
#include "opengl_widget.hpp"
#include "status_bar.hpp"
#include "tool_bar/camera.hpp"
#include "tool_bar/snapshot.hpp"
#include <QDialog>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
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
			createMenu<Menu::View>();
			createMenu<Menu::Help>();

			// Toolbars.
			createToolBar<ToolBar::Camera>();
			createToolBar<ToolBar::Snapshot>();

			// Main area : opengl widget.
			//_openGLWidget = new OpenGLWidget( this );
			// setCentralWidget( _openGLWidget );
			auto * centerPanel = new QDockWidget( "Renderer" );
			setCentralWidget( centerPanel );

			// Dock widgets.
			createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );
			createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::RenderSettings>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Representations>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
			createDockWidget<DockWidget::Sequence>( Qt::TopDockWidgetArea );

			// Status bar.
			_statusBar = new StatusBar( this );
			setStatusBar( _statusBar );

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

	  private:
		QPointer<OpenGLWidget> _openGLWidget;
		QPointer<StatusBar>	   _statusBar;
	};

} // namespace VTX::UI::QT

#endif
