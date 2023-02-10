#ifndef __VTX_UI_QT_MAIN_WINDOW__
#define __VTX_UI_QT_MAIN_WINDOW__

#include "core/base_main_window.hpp"
#include "core/base_panel.hpp"
#include "core/define.hpp"
#include "qt/contextual_menu.hpp"
#include "qt/qt_panel.hpp"
#include "qt/widget/main_menu/main_menu_bar.hpp"
#include "ui_main_window.h"
#include <QCloseEvent>
#include <QDockWidget>
#include <QKeySequence>
#include <QMainWindow>
#include <QShortcut>
#include <old/event/base_event_firerer_input.hpp>
#include <old/ui/cursor_handler.hpp>
//  #include <old/ui/widget/analysis/structural_alignment/structural_alignment_widget.hpp>
#include "widget/base_widget.hpp"
//  #include <old/ui/widget/console/console_widget.hpp>
//  #include <old/ui/widget/information/information_widget.hpp>
//  #include <old/ui/widget/inspector/inspector_widget.hpp>
#include <old/id.hpp>
#include <old/renderer/base_renderer.hpp>
//  #include <old/ui/widget/render/opengl_widget.hpp>
//  #include <old/ui/widget/scene/scene_widget.hpp>
//  #include <old/ui/widget/selection/selection_widget.hpp>
//  #include <old/ui/widget/sequence/sequence_widget.hpp>
//  #include <old/ui/widget/settings/setting_widget.hpp>
//  #include <old/ui/widget/settings/setting_widget_enum.hpp>
//  #include <old/ui/widget/status_bar/status_bar_widget.hpp>
#include <unordered_set>
#include <util/types.hpp>

namespace VTX::UI::QT
{
	namespace Tool::Render::Widget
	{
		class RenderWidget;
	}

	class MainWindow :
		public VTX::UI::QT::Widget::BaseWidget<QMainWindow, Ui_MainWindow>,
		public VTX::UI::Core::BaseMainWindow,
		public Event::BaseEventFirererInput
	{
		Q_OBJECT

	  private:
		inline static const int QT_UNKNOWN_WIDGET_DEFAULT_LAYOUT_HEIGHT = 10;

	  public:
		MainWindow( QWidget * p_parent = nullptr );
		~MainWindow();

		void setupUi();

		void initWindowLayout();
		void refreshWindowTitle();

		virtual void referencePanel( const Core::WidgetKey & p_key, Core::BasePanel * const p_panel ) override;

		QT::Tool::Render::Widget::RenderWidget *			 getRender();
		const QT::Tool::Render::Widget::RenderWidget * const getRender() const;

		bool isOpenGLValid() const;
		void updateRender() const;
		void updateRenderSetting( const Renderer::RENDER_SETTING );

		void receiveEvent( const Event::VTXEvent & p_event ) override;

		Core::MainMenu::MainMenuBar & getMainMenu() override { return *_mainMenuBar; }

		void addShortcut( const std::string & p_shortcut, QAction * const p_action );

		template<typename Obj, typename Func>
		void addShortcut( const std::string & p_shortcut, Obj p_obj, Func p_func )
		{
			assert( _shortcuts.find( p_shortcut ) == _shortcuts.end() );

			connect(
				new QShortcut( QKeySequence( tr( p_shortcut.c_str() ) ), this ), &QShortcut::activated, p_obj, p_func );

			_shortcuts.emplace( p_shortcut );
		}

		const ContextualMenu & getContextualMenu() const { return *_contextualMenu; }
		ContextualMenu &	   getContextualMenu() { return *_contextualMenu; }
		CursorHandler &		   getCursorHandler() { return *_cursorHandler; }

		bool getWidgetVisibility( const ID::VTX_ID & p_winId ) const;
		void showWidget( const ID::VTX_ID & p_winId, const bool p_show ) const;
		void toggleWidget( const ID::VTX_ID & p_winId ) const;
		// void openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const;

		Core::WindowMode getWindowMode();
		void			 setWindowMode( const Core::WindowMode & p_mode );
		void			 toggleWindowState();

		bool hasValidLayoutSave() const;
		void loadLastLayout();
		void saveLayout() const;
		void deleteLayoutSaveFile() const;
		void restoreDefaultLayout();

		void addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
									  Qt::DockWidgetArea  p_area,
									  Qt::Orientation	  p_orientation,
									  const bool		  p_visible = true );
		void addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
									  QDockWidget * const p_neighbour,
									  Qt::Orientation	  p_orientation,
									  const bool		  p_visible );
		void addDockWidgetAsFloating( QDockWidget * const p_dockWidget, const QSize & p_size, const bool p_visible );

		QWidget & getWidget( const ID::VTX_ID & p_winId ) const;
		template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
		W & getWidget( const ID::VTX_ID & p_winId ) const
		{
			return static_cast<W &>( getWidget( p_winId ) );
		}

		const ID::VTX_ID getEventFirererId() const override { return ID::UI::Input::MAIN_WINDOW; };

	  protected:
		void resizeEvent( QResizeEvent * ) override;
		void showEvent( QShowEvent * ) override;
		void dragEnterEvent( QDragEnterEvent * ) override;
		void dropEvent( QDropEvent * ) override;
		void closeEvent( QCloseEvent * ) override;
		void changeEvent( QEvent * p_event ) override;

		void keyPressEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }
		void keyReleaseEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }

		virtual bool eventFilter( QObject * const p_watched, QEvent * const p_event ) override;

	  private:
		QT::Widget::MainMenu::MainMenuBar * _mainMenuBar = nullptr;

		std::unordered_set<std::string> _shortcuts = std::unordered_set<std::string>();

		// Widget::Render::RenderWidget *			 _renderWidget		= nullptr;
		// Widget::Scene::SceneWidget *			 _sceneWidget		= nullptr;
		// Widget::Inspector::InspectorWidget *	 _inspectorWidget	= nullptr;
		// Widget::Console::ConsoleWidget *		 _consoleWidget		= nullptr;
		// Widget::Sequence::SequenceWidget *		 _sequenceWidget	= nullptr;
		// Widget::Selection::SelectionWidget *	 _selectionWidget	= nullptr;
		// Widget::Settings::SettingWidget *		 _settingWidget		= nullptr;
		// Widget::Information::InformationWidget * _informationWidget = nullptr;

		// Widget::Analysis::StructuralAlignment::StructuralAlignmentWidget * _structuralAlignmentWidget = nullptr;

		ContextualMenu * _contextualMenu = nullptr;
		CursorHandler *	 _cursorHandler	 = nullptr;

		// Widget::StatusBar::StatusBarWidget * _statusBarWidget = nullptr;

		// Actions.
		void _onDockWindowVisibilityChange( bool p_visible );

		void _updatePicker() const;

		// Functions.
		void _loadStyleSheet( const char * p_stylesheetPath );
		void _setupSlots();
		void _restoreDockWidget( QDockWidget * const p_dockWidget );

		// Shortcuts.
		void _onShortcutFullscreen() const;
		void _onShortcutClearSelection() const;
		void _onShortcutRestoreLayout() const;
		void _onShortcutCompileShaders() const;
		void _onShortcutActiveRenderer() const;
		void _onShortcutDelete() const;
		void _onShortcutOrient() const;
		void _onShortcutSelectAll() const;
		void _onShortcutCopy() const;
		void _onShortcutExtract() const;
		void _onShortcutSetSelectionPicker() const;
		void _onShortcutSetMeasurementPicker() const;

		Core::WindowMode _getWindowModeFromWindowState( const Qt::WindowStates & p_state );
		std::string		 _getWindowTitle() const;

		void _delayRestoreState();
		void _restoreStateDelayedAction();

		// When we restore a layout where a windows has not been set (new feature since the last launch ?), the
		// window has a default size of 0. This function will check that and display it at the preferred size of the
		// window.
		void _checkUnknownFloatableWindows();
		void _checkUnknownFloatableWindow( QDockWidget * const p_widget, const QSize & p_defaultSize );

		QTimer * _restoreStateTimer;
	};

} // namespace VTX::UI::QT

#endif
