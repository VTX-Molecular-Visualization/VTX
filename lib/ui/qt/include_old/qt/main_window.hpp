#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "core/main_menu/main_menu_bar.hpp"
#include "core/main_menu/menu_toolblock_widget.hpp"
#include "core/main_menu/menu_tooltab_widget.hpp"
#include "core/status/status_bar.hpp"
#include "qt_panel.hpp"
#include "ui/core/base_main_window.hpp"
#include "ui/core/base_panel.hpp"
#include "ui/core/define.hpp"
#include "ui/core/layout_descriptor.hpp"
#include "widget/renderer/panel.hpp"
#include <QCloseEvent>
#include <QDockWidget>
#include <QKeySequence>
#include <QMainWindow>
#include <unordered_set>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	namespace Tool::Render::Widget
	{
		class RenderWidget;
	}

	class MainWindow : public VTX::UI::QT::Core::BaseManualWidget<QMainWindow>, public VTX::UI::Core::BaseMainWindow
	{
		Q_OBJECT

	  private:
		inline static const int QT_UNKNOWN_WIDGET_DEFAULT_LAYOUT_HEIGHT = 10;

	  public:
		MainWindow( QWidget * p_parent = nullptr );
		~MainWindow();

		void localize() override;

		void initWindowLayout();
		void refreshWindowTitle();

		QT::Widget::Renderer::Panel *			  getRender();
		const QT::Widget::Renderer::Panel * const getRender() const;

		Core::MainMenu::MainMenuBar & getMainMenu() override { return *_mainMenuBar; }
		// QT::Core::MainMenu::MenuTooltabWidget &	  getMainMenuToolTab( const UI::Core::ToolLayoutData & layoutData );
		// QT::Core::MainMenu::MenuToolBlockWidget & getMainMenuToolBlock( const UI::Core::ToolLayoutData & layoutData
		// );

		UI::Core::WindowMode getWindowMode();
		void				 setWindowMode( const UI::Core::WindowMode & p_mode );
		void				 toggleWindowState();

		void addDockWidgetAsTabified(
			QDockWidget * const p_dockWidget,
			Qt::DockWidgetArea	p_area,
			Qt::Orientation		p_orientation,
			const bool			p_visible = true
		);
		void addDockWidgetAsTabified(
			QDockWidget * const p_dockWidget,
			QDockWidget * const p_neighbour,
			Qt::Orientation		p_orientation,
			const bool			p_visible
		);
		void addDockWidgetAsFloating( QDockWidget * const p_dockWidget, const QSize & p_size, const bool p_visible );

		void addFloatingWindow( QDialog * const p_window, const QSize & p_size, const bool p_visible );

		// void appendStylesheet( const char * p_stylesheetPath );

	  protected:
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void keyPressEvent( QKeyEvent * const p_event ) override;
		void keyReleaseEvent( QKeyEvent * const p_event ) override;
		void mousePressEvent( QMouseEvent * const p_event ) override;
		void mouseReleaseEvent( QMouseEvent * const p_event ) override;
		void mouseDoubleClickEvent( QMouseEvent * const p_event ) override;
		void mouseMoveEvent( QMouseEvent * const p_event ) override;
		void wheelEvent( QWheelEvent * const p_event ) override;

		void changeEvent( QEvent * const p_event ) override;
		void closeEvent( QCloseEvent * const p_event ) override;

	  private:
		QT::Core::MainMenu::MainMenuBar * _mainMenuBar;
		QT::Core::Status::StatusBar *	  _statusBar;

		// Actions.
		void _onDockWindowVisibilityChange( bool p_visible );

		void _updatePicker() const;

		UI::Core::WindowMode _getWindowModeFromWindowState( const Qt::WindowStates & p_state );
		std::string			 _getWindowTitle() const;

		void _delayRestoreState();
		void _restoreStateDelayedAction();

		// When we restore a layout where a windows has not been set (new feature since the last launch ?), the
		// window has a default size of 0. This function will check that and display it at the preferred size of the
		// window.
		void _checkUnknownFloatableWindows();
		void _checkUnknownFloatableWindow( QDockWidget * const p_widget, const QSize & p_defaultSize );

		QTimer * _restoreStateTimer = nullptr;
	};

} // namespace VTX::UI::QT

#endif