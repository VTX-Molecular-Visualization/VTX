#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "dock_widget/console.hpp"
#include "menu/file.hpp"
#include "menu/help.hpp"
#include "menu/view.hpp"
#include "tool_bar/camera.hpp"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolbar>
#include <ui/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	class MainWindow : public QMainWindow //, public BaseMainWindow
	{
		Q_OBJECT

	  public:
		MainWindow() : QMainWindow()
		{
			// Size.
			resize( 1920, 1080 );

			setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		}

		virtual ~MainWindow() {}

		void build()
		{
			// Main menu.
			_createMenu<Menu::File>();
			_createMenu<Menu::View>();
			_createMenu<Menu::View>();
			_createMenu<Menu::Help>();

			// Toolbars.
			_createToolBar<ToolBar::Camera>();

			// Snapshot.
			QToolBar * editToolBar = addToolBar( "Snapshot" );
			editToolBar->addAction( new QAction( "Quick snapshot" ) );
			editToolBar->addAction( new QAction( "Export image" ) );

			// Viewpoints.
			QToolBar * viewpointsToolBar = addToolBar( "Viewpoints" );
			viewpointsToolBar->addAction( new QAction( "Create" ) );
			viewpointsToolBar->addAction( new QAction( "Delete" ) );

			// Render settings.
			QToolBar * renderToolBar = addToolBar( "Render settings" );
			renderToolBar->addAction( new QAction( "Background color" ) );
			renderToolBar->addAction( new QAction( "Preset" ) );

			// Main area : opengl widget.
			setCentralWidget( new QWidget( this ) );

			QDockWidget * dockWidget = new QDockWidget( "Scene", this );
			dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			dockWidget->setFeatures(
				QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable
			);
			addDockWidget( Qt::LeftDockWidgetArea, dockWidget );

			// Dock widgets.
			dockWidget = new QDockWidget( "Inspector", this );
			dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			addDockWidget( Qt::RightDockWidgetArea, dockWidget );

			dockWidget = new QDockWidget( "Render settings", this );
			dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			addDockWidget( Qt::RightDockWidgetArea, dockWidget );

			dockWidget = new QDockWidget( "Representations", this );
			dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			addDockWidget( Qt::RightDockWidgetArea, dockWidget );

			dockWidget = new QDockWidget( "Options", this );
			dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			addDockWidget( Qt::RightDockWidgetArea, dockWidget );

			_createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );

			dockWidget = new QDockWidget( "Sequence", this );
			dockWidget->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
			dockWidget->setWidget( new QWidget( this ) );
			addDockWidget( Qt::TopDockWidgetArea, dockWidget );

			// Status bar.
			QStatusBar * status = statusBar();
			status->showMessage( "Ready" );
		}

		void addMenuAction( const MenuAction & p_ma )
		{
			for ( QMenu * const menu : menuBar()->findChildren<QMenu *>() )
			{
				if ( menu->title().toStdString() == p_ma.idMenu )
				{
					// TODO: connect action/callback.
					menu->addAction( new QAction( QString::fromStdString( p_ma.name ) ) );
					return;
				}
			}

			throw std::runtime_error( "Menu not found." );
		}

		void addToolBarAction( const ToolBarAction & p_tba )
		{
			for ( QToolBar * const toolbar : findChildren<QToolBar *>() )
			{
				if ( toolbar->windowTitle().toStdString() == p_tba.idToolBar )
				{
					// TODO: connect action/callback.
					toolbar->addAction( new QAction( QString::fromStdString( p_tba.name ) ) );
					return;
				}
			}

			throw std::runtime_error( "Tool bar not found." );
		}

	  private:
		template<typename M>
		void _createMenu()
		{
			menuBar()->addMenu( new M( menuBar() ) );
		}

		template<typename TB>
		void _createToolBar()
		{
			addToolBar( new TB( this ) );
		}

		template<typename DW>
		void _createDockWidget( const Qt::DockWidgetArea & p_area )
		{
			addDockWidget( p_area, new DW( this ) );
		}
	};

} // namespace VTX::UI::QT

#endif
