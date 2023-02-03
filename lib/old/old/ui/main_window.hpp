#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#include "contextual_menu.hpp"
#include "cursor_handler.hpp"
#include "ui_main_window.h"
#include "widget/analysis/structural_alignment/structural_alignment_widget.hpp"
#include "widget/base_widget.hpp"
#include "widget/console/console_widget.hpp"
#include "widget/information/information_widget.hpp"
#include "widget/inspector/inspector_widget.hpp"
#include "widget/main_menu/main_menu_bar.hpp"
#include "widget/render/opengl_widget.hpp"
#include "widget/render/render_widget.hpp"
#include "widget/scene/scene_widget.hpp"
#include "widget/selection/selection_widget.hpp"
#include "widget/sequence/sequence_widget.hpp"
#include "widget/settings/setting_widget.hpp"
#include "widget/settings/setting_widget_enum.hpp"
#include "widget/status_bar/status_bar_widget.hpp"
#include <QCloseEvent>
#include <QDockWidget>
#include <QMainWindow>

namespace VTX
{
	namespace UI
	{
		enum class WindowMode
		{
			Fullscreen,
			Windowed,
			Maximized,
			Minimized,
		};

		class MainWindow : public Widget::BaseWidget<QMainWindow, Ui_MainWindow>, public Event::BaseEventFirererInput
		{
			Q_OBJECT

		  private:
			inline static const int QT_UNKNOWN_WIDGET_DEFAULT_LAYOUT_HEIGHT = 10;

		  public:
			MainWindow( QWidget * = 0 );
			~MainWindow();
			void setupUi();
			void initWindowLayout();

			void refreshWindowTitle();

			inline bool isOpenGLValid() const { return _renderWidget->isOpenGLValid(); }
			inline void updateRender() const { _renderWidget->updateRender(); }
			void		updateRenderSetting( const Renderer::RENDER_SETTING );
			const Vec2i getPickedIds( const uint p_x, const uint p_y );

			void receiveEvent( const Event::VTXEvent & p_event ) override;

			Widget::MainMenu::MainMenuBar & getMainMenu() { return *_mainMenuBar; };

			const ContextualMenu & getContextualMenu() { return *_contextualMenu; }
			CursorHandler &		   getCursorHandler() { return *_cursorHandler; }

			bool getWidgetVisibility( const ID::VTX_ID & p_winId ) const;
			void showWidget( const ID::VTX_ID & p_winId, const bool p_show ) const;
			void toggleWidget( const ID::VTX_ID & p_winId ) const;
			void openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const;

			WindowMode getWindowMode();
			void	   setWindowMode( const WindowMode & p_mode );
			void	   toggleWindowState();

			bool hasValidLayoutSave() const;
			void loadLastLayout();
			void saveLayout() const;
			void deleteLayoutSaveFile() const;
			void restoreDefaultLayout();

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
			Widget::MainMenu::MainMenuBar * _mainMenuBar = nullptr;

			Widget::Render::RenderWidget *			 _renderWidget		= nullptr;
			Widget::Scene::SceneWidget *			 _sceneWidget		= nullptr;
			Widget::Inspector::InspectorWidget *	 _inspectorWidget	= nullptr;
			Widget::Console::ConsoleWidget *		 _consoleWidget		= nullptr;
			Widget::Sequence::SequenceWidget *		 _sequenceWidget	= nullptr;
			Widget::Selection::SelectionWidget *	 _selectionWidget	= nullptr;
			Widget::Settings::SettingWidget *		 _settingWidget		= nullptr;
			Widget::Information::InformationWidget * _informationWidget = nullptr;

			Widget::Analysis::StructuralAlignment::StructuralAlignmentWidget * _structuralAlignmentWidget = nullptr;

			ContextualMenu * _contextualMenu = nullptr;
			CursorHandler *	 _cursorHandler	 = nullptr;

			Widget::StatusBar::StatusBarWidget * _statusBarWidget = nullptr;

			// Actions.
			void _onDockWindowVisibilityChange( bool p_visible );

			void _updatePicker() const;

			// Functions.
			void _loadStyleSheet( const char * p_stylesheetPath );
			void _setupSlots();
			void _restoreDockWidget( QDockWidget * const p_dockWidget );
			void _addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
										   Qt::DockWidgetArea  p_area,
										   Qt::Orientation	   p_orientation,
										   const bool		   p_visible = true );
			void _addDockWidgetAsTabified( QDockWidget * const p_dockWidget,
										   QDockWidget * const p_neighbour,
										   Qt::Orientation	   p_orientation,
										   const bool		   p_visible );
			void _addDockWidgetAsFloating( QDockWidget * const p_dockWidget,
										   const QSize &	   p_size,
										   const bool		   p_visible );

			// Shortcuts.
			void _onShortcutNew() const;
			void _onShortcutOpen() const;
			void _onShortcutSave() const;
			void _onShortcutSaveAs() const;
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

			WindowMode _getWindowModeFromWindowState( const Qt::WindowStates & p_state );

			void _delayRestoreState();
			void _restoreStateDelayedAction();

			// When we restore a layout where a windows has not been set (new feature since the last launch ?), the
			// window has a default size of 0. This function will check that and display it at the preferred size of the
			// window.
			void _checkUnknownFloatableWindows();
			void _checkUnknownFloatableWindow( QDockWidget * const p_widget, const QSize & p_defaultSize );

			QTimer * _restoreStateTimer;
		};

	} // namespace UI
} // namespace VTX

#endif
