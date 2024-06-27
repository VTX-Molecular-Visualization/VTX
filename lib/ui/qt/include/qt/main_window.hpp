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
#include "status_bar.hpp"
#include "tool_bar/camera.hpp"
#include "tool_bar/snapshot.hpp"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <ui/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	class MainWindow : public BaseWidget<MainWindow, QMainWindow>
	{
	  public:
		MainWindow() : BaseWidget<MainWindow, QMainWindow>(), _statusBar( this )
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
			setCentralWidget( new QWidget( this ) );

			// Dock widgets.
			createDockWidget<DockWidget::Console>( Qt::BottomDockWidgetArea );
			createDockWidget<DockWidget::Inspector>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Options>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::RenderSettings>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Representations>( Qt::RightDockWidgetArea );
			createDockWidget<DockWidget::Scene>( Qt::LeftDockWidgetArea );
			createDockWidget<DockWidget::Sequence>( Qt::TopDockWidgetArea );

			// Status bar.
			setStatusBar( &_statusBar );
			_statusBar.showMessage( "Ready" );
		}

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

			throw std::runtime_error( "Menu not found." );
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

			throw std::runtime_error( "Tool bar not found." );
		}

		template<typename M>
		void createMenu()
		{
			menuBar()->addMenu( new M( menuBar() ) );
		}

		template<typename TB>
		void createToolBar()
		{
			addToolBar( new TB( this ) );
		}

		template<typename DW>
		void createDockWidget( const Qt::DockWidgetArea & p_area )
		{
			addDockWidget( p_area, new DW( this ) );
		}

	  private:
		StatusBar _statusBar;
	};

} // namespace VTX::UI::QT

#endif
