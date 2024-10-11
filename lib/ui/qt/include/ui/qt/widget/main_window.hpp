#ifndef __VTX_UI_QT_WIDGET_MAIN_WINDOW__
#define __VTX_UI_QT_WIDGET_MAIN_WINDOW__

#include "opengl_widget.hpp"
#include "status_bar.hpp"
#include "ui/qt/base_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
#include <app/ui/concepts.hpp>
// #include <ui/qt/dialog/progress.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Dialog
{
	class Progress;
}

namespace VTX::UI::QT::Widget
{

	class MainWindow : public BaseWidget<MainWindow, QMainWindow>, public Savable
	{
	  public:
		MainWindow();

		void prepare();
		void build();
		void addMenuAction( const App::UI::WidgetId & p_menu, const App::UI::DescAction & p_action );
		void addToolBarAction( const App::UI::WidgetId & p_toolbar, const App::UI::DescAction & p_action );
		void resetLayout();

		inline void setClosing( const bool p_closing ) { _closing = p_closing; }

		void changeEvent( QEvent * const p_event ) override;
		void closeEvent( QCloseEvent * p_event ) override;

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
		DW * createDockWidget( const Qt::DockWidgetArea p_area )
		{
			QDockWidget * other = nullptr;
			for ( QDockWidget * w : findChildren<QDockWidget *>() )
			{
				if ( dockWidgetArea( w ) == p_area )
				{
					other = w;
				}
			}

			DW * const dockWidget = new DW( this );
			addDockWidget( p_area, dockWidget );

			// Tabify.
			if ( other != nullptr )
			{
				tabifyDockWidget( other, dockWidget );
			}

			return dockWidget;
		}

		void save() override;
		void restore() override;

	  private:
		QPointer<OpenGLWidget>	   _openGLWidget;
		QPointer<StatusBar>		   _statusBar;
		QPointer<Dialog::Progress> _progressDialog;

		// TODO: keep like that or re-tabify?
		QByteArray _defaultGeometry;
		QByteArray _defaultState;

		bool _closing = false;
	};

} // namespace VTX::UI::QT::Widget

#endif
