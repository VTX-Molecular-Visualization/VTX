#ifndef __VTX_UI_QT_WIDGET_MAIN_WINDOW__
#define __VTX_UI_QT_WIDGET_MAIN_WINDOW__

#include "opengl_widget.hpp"
#include "status_bar.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
#include <app/events.hpp>
#include <app/ui/concepts.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Dialog
{
	class TrajectoryAssociation;
	class Progress;
} // namespace VTX::UI::QT::Dialog

namespace VTX::UI::QT::Widget
{

	/**
	 * @brief The application main window.
	 */
	class MainWindow : public Widget::BaseWidget<MainWindow, QMainWindow>
	{
	  public:
		/**
		 * @brief Build widgets.
		 */
		MainWindow();

		/**
		 * @brief Destructor.
		 */
		~MainWindow();

		/**
		 * @brief Hook an action to the given menu.
		 */
		void addMenuAction( const App::UI::WidgetId & p_menu, const App::UI::DescAction & p_action );

		/**
		 * @brief Hook an action to the given toolbar.
		 */
		void addToolBarAction( const App::UI::WidgetId & p_toolbar, const App::UI::DescAction & p_action );

		/**
		 * @brief Reset layout to default state.
		 */
		void resetLayout();

		uintptr_t getNativeSurface() const;
		uintptr_t getNativeDisplay() const;
		uint8_t	  getNativePlatform() const;

		/**
		 * @brief Catch close event to quit application.
		 */
		void closeEvent( QCloseEvent * ) override;

		/**
		 * @brief Create a menu from type.
		 */
		template<typename M>
		M * const createMenu()
		{
			M * const menu = new M( menuBar() );
			menuBar()->addMenu( menu );
			return menu;
		}

		/**
		 * @brief Create a toolbar from type.
		 */
		template<typename TB>
		TB * const createToolBar()
		{
			TB * const toolBar = new TB( this );
			addToolBar( toolBar );
			return toolBar;
		}

		/**
		 * @brief Create a dock widget from type in the given area.
		 */
		template<typename DW>
		DW * const createDockWidget( const Qt::DockWidgetArea p_area )
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

	  protected:
		/**
		 * @brief Drag and drop events.
		 */
		void dragEnterEvent( QDragEnterEvent * );
		void dropEvent( QDropEvent * );

	  private:
		/**
		 * @brief The bottom status bar.
		 */
		QPointer<StatusBar> _statusBar;

		/**
		 * @brief The progress dialog shown during blocking operations.
		 */
		QPointer<Dialog::Progress> _progressDialog;

		/**
		 * @brief Default geometry for layout reset.
		 */
		QByteArray _defaultGeometry;

		/**
		 * @brief Default state for layout reset.
		 */
		QByteArray _defaultState;

		/**
		 * @brief Event handlers for app operations.
		 */
		void _onApplicationError( const App::Events::ApplicationError & );
		void _onBlockingOperationStart( const App::Events::BlockingOperationStart & );
		void _onBlockingOperationProgress( const App::Events::BlockingOperationProgress & );
		void _onBlockingOperationEnd( const App::Events::BlockingOperationEnd & );
		void _onUpdateAvailable( const App::Events::UpdateAvailable & );
	};

} // namespace VTX::UI::QT::Widget

#endif
