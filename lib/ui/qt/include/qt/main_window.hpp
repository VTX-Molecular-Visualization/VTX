#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "base_widget.hpp"
#include "opengl_widget.hpp"
#include "status_bar.hpp"
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
		MainWindow();
		virtual ~MainWindow() {}

		void build();
		void init();
		void addMenuAction( const WidgetId & p_menu, const UI::Action & p_action );
		void addToolBarAction( const WidgetId & p_toolbar, const UI::Action & p_action );

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

		void resetLayout();

	  private:
		QPointer<OpenGLWidget> _openGLWidget;
		QPointer<StatusBar>	   _statusBar;

		// TODO: keep like that or re-tabify?
		QByteArray _defaultGeometry;
		QByteArray _defaultState;
	};

} // namespace VTX::UI::QT

#endif