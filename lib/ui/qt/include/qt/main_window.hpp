#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "base_widget.hpp"
#include "helper.hpp"
#include "opengl_widget.hpp"
#include "status_bar.hpp"
#include "ui/concepts.hpp"
#include <QMainWindow>
#include <QMenuBar>
#include <QPointer>
#include <app/core/input/input_manager.hpp>
#include <ui/actions.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	class MainWindow : public BaseWidget<MainWindow, QMainWindow>
	{
	  public:
		MainWindow();
		virtual ~MainWindow() {}

		void build();
		void addMenuAction( const WidgetId & p_menu, const UI::DescAction & p_action );
		void addToolBarAction( const WidgetId & p_toolbar, const UI::DescAction & p_action );
		void resetLayout();

		inline void keyPressEvent( QKeyEvent * const p_event ) override
		{
			_inputManager.handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		inline void keyReleaseEvent( QKeyEvent * const p_event ) override
		{
			_inputManager.handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		inline void mousePressEvent( QMouseEvent * const p_event ) override
		{
			_inputManager.handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseReleaseEvent( QMouseEvent * const p_event ) override
		{
			_inputManager.handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseDoubleClickEvent( QMouseEvent * const p_event ) override
		{
			_inputManager.handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseMoveEvent( QMouseEvent * const p_event ) override
		{
			_inputManager.handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void wheelEvent( QWheelEvent * const p_event ) override
		{
			_inputManager.handleMouseWheelEvent( Helper::qWheelEventToWheelEvent( *p_event ) );
		}

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
		DW * createDockWidget( const Qt::DockWidgetArea & p_area )
		{
			DW * const dockWidget = new DW( this );
			addDockWidget( p_area, dockWidget );
			return dockWidget;
		}

	  private:
		QPointer<OpenGLWidget> _openGLWidget;
		QPointer<StatusBar>	   _statusBar;

		// TODO: keep like that or re-tabify?
		QByteArray _defaultGeometry;
		QByteArray _defaultState;

		App::Core::Input::InputManager & _inputManager;
	};

} // namespace VTX::UI::QT

#endif
