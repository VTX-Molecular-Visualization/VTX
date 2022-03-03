#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#include "contextual_menu.hpp"
#include "cursor_handler.hpp"
#include "ui_main_window.h"
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

		  public:
			MainWindow( QWidget * = 0 );
			~MainWindow();
			void setupUi();
			void initWindowLayout();

			void refreshWindowTitle();

			inline const Widget::Render::RenderWidget & getRenderWidget() const { return *_renderWidget; }
			inline Widget::Render::RenderWidget &		getRenderWidget() { return *_renderWidget; }

			inline const Widget::Render::OpenGLWidget & getOpenGLWidget() const
			{
				return _renderWidget->getOpenGLWidget();
			}
			inline Widget::Render::OpenGLWidget & getOpenGLWidget() { return _renderWidget->getOpenGLWidget(); }

			void receiveEvent( const Event::VTXEvent & p_event ) override;

			const ContextualMenu & getContextualMenu() { return *_contextualMenu; }
			CursorHandler &		   getCursorHandler() { return *_cursorHandler; }

			bool getWidgetVisibility( const ID::VTX_ID & p_winId ) const;
			void showWidget( const ID::VTX_ID & p_winId, const bool p_show ) const;
			void toggleWidget( const ID::VTX_ID & p_winId ) const;
			void openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const;

			bool isOpenGLValid() const;

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

			ContextualMenu * _contextualMenu = nullptr;
			CursorHandler *	 _cursorHandler	 = nullptr;

			Widget::StatusBar::StatusBarWidget * _statusBarWidget = nullptr;

			// Actions.
			void _onDockWindowVisibilityChange( bool p_visible );

			void _updatePicker() const;

			// Functions.
			void _loadStyleSheet( const char * p_stylesheetPath );
			void _setupSlots();

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

			void	 _delayRestoreState();
			void	 _restoreStateDelayedAction();
			QTimer * _restoreStateTimer;
		};

	} // namespace UI
} // namespace VTX

#endif
