#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "ui/core/base_main_window.hpp"
#include "ui/core/base_panel.hpp"
#include "ui/core/define.hpp"
#include "ui/core/layout_descriptor.hpp"
#include "ui/qt/qt_panel.hpp"
#include "ui/qt/widget/main_menu/main_menu_bar.hpp"
#include "ui/qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/widget/main_menu/menu_tooltab_widget.hpp"
#include <QCloseEvent>
#include <QDockWidget>
#include <QKeySequence>
#include <QMainWindow>
#include <QShortcut>
#include <unordered_set>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	namespace Tool::Render::Widget
	{
		class RenderWidget;
	}

	class MainWindow : public VTX::UI::QT::Widget::BaseManualWidget<QMainWindow>, public VTX::UI::Core::BaseMainWindow
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

		QT::Tool::Render::Widget::RenderWidget *			 getRender();
		const QT::Tool::Render::Widget::RenderWidget * const getRender() const;

		Core::MainMenu::MainMenuBar &				getMainMenu() override { return *_mainMenuBar; }
		QT::Widget::MainMenu::MenuTooltabWidget &	getMainMenuToolTab( const Core::ToolLayoutData & layoutData );
		QT::Widget::MainMenu::MenuToolBlockWidget & getMainMenuToolBlock( const Core::ToolLayoutData & layoutData );

		void addShortcut( const std::string & p_shortcut, QAction * const p_action );

		template<typename Obj, typename Func>
		void addShortcut( const std::string & p_shortcut, Obj p_obj, Func p_func )
		{
			assert( _shortcuts.find( p_shortcut ) == _shortcuts.end() );

			connect(
				new QShortcut( QKeySequence( tr( p_shortcut.c_str() ) ), this ), &QShortcut::activated, p_obj, p_func );

			_shortcuts.emplace( p_shortcut );
		}

		Core::WindowMode getWindowMode();
		void			 setWindowMode( const Core::WindowMode & p_mode );
		void			 toggleWindowState();

		void addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
									  Qt::DockWidgetArea  p_area,
									  Qt::Orientation	  p_orientation,
									  const bool		  p_visible = true );
		void addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
									  QDockWidget * const p_neighbour,
									  Qt::Orientation	  p_orientation,
									  const bool		  p_visible );
		void addDockWidgetAsFloating( QDockWidget * const p_dockWidget, const QSize & p_size, const bool p_visible );

		void addFloatingWindow( QDialog * const p_window, const QSize & p_size, const bool p_visible );

		void appendStylesheet( const char * p_stylesheetPath );

	  protected:
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QT::Widget::MainMenu::MainMenuBar * _mainMenuBar = nullptr;

		std::unordered_set<std::string> _shortcuts = std::unordered_set<std::string>();

		// Actions.
		void _onDockWindowVisibilityChange( bool p_visible );

		void _updatePicker() const;

		// Functions.
		void _loadStyleSheet( const char * p_stylesheetPath );

		Core::WindowMode _getWindowModeFromWindowState( const Qt::WindowStates & p_state );
		std::string		 _getWindowTitle() const;

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
