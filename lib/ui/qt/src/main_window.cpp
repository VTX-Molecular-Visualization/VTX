#include "main_window.hpp"
#include "application_qt.hpp"
#include "util.hpp"
#include "widget/renderer_widget.hpp"
#include "widget_factory.hpp"
#include <app/application/scene.hpp>
#include <app/core/input/input_manager.hpp>
#include <io/internal/filesystem.hpp>

namespace VTX::UI::QT
{
	MainWindow::MainWindow( QWidget * p_parent ) : BaseMainWindow(), BaseManualWidget( p_parent ) {}

	MainWindow::~MainWindow() {}

	void MainWindow::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		const QSize winsize = QSize( 1280, 720 );
		resize( winsize );
		setWindowState( Qt::WindowState::WindowNoState );
		refreshWindowTitle();
		setContextMenuPolicy( Qt::ContextMenuPolicy::PreventContextMenu );

		_mainMenuBar = WidgetFactory::get().instantiateWidget<QT::Core::MainMenu::MainMenuBar>( this, "mainMenuBar" );
		setMenuBar( _mainMenuBar );

		_statusBar = WidgetFactory::get().instantiateWidget<QT::Core::Status::StatusBar>( this, "statusBar" );
		setStatusBar( _statusBar );

		setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

		_loadTheme();
	}

	void MainWindow::initWindowLayout() {}

	QT::Core::MainMenu::MenuTooltabWidget & MainWindow::getMainMenuToolTab( const UI::Core::ToolLayoutData & layoutData
	)
	{
		return dynamic_cast<QT::Core::MainMenu::MenuTooltabWidget &>( getMainMenu().getTab( layoutData.tabName ) );
	}
	QT::Core::MainMenu::MenuToolBlockWidget & MainWindow::getMainMenuToolBlock(
		const UI::Core::ToolLayoutData & layoutData
	)
	{
		return dynamic_cast<VTX::UI::QT::Core::MainMenu::MenuToolBlockWidget &>(
			getMainMenu().getTab( layoutData.tabName ).getToolBlock( layoutData.blockName )
		);
	}

	void MainWindow::_loadTheme()
	{
		// TODO: move to style constants.
		// Load main stylesheet.
		QFile stylesheetFile( ":/stylesheet_ui.css" );
		stylesheetFile.open( QFile::ReadOnly );

		QString stylesheet = stylesheetFile.readAll();

		// Load os-specific stylesheet.
#if _WIN32
		QFile stylesheetOSFile( ":/stylesheet_ui_windows.css" );
#elif __linux__
		QFile stylesheetOSFile( ":/stylesheet_ui_linux.css" );
#elif __APPLE__
		QFile stylesheetOSFile( ":/stylesheet_ui_mac.css" );
#else
		QFile stylesheetOSFile();
		assert( true );
#endif

		stylesheetOSFile.open( QFile::ReadOnly );
		stylesheet += '\n' + stylesheetOSFile.readAll();

		// Load theme and apply to stylesheet.

		// Set stylesheet to app.
		setStyleSheet( stylesheet );
	}

	/*
	void MainWindow::appendStylesheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		const QString stylesheetContent = stylesheetFile.readAll();
		const QString newStylesheet		= styleSheet() + '\n' + stylesheetContent;

		setStyleSheet( newStylesheet );
	}
	*/

	void MainWindow::_setupSlots() {}
	void MainWindow::localize() { setWindowTitle( "VTX" ); }

	QT::Widget::Renderer::Panel * MainWindow::getRender()
	{
		return getPanel<QT::Widget::Renderer::Panel>( "RENDER_WINDOW_KEY" );
	}
	const QT::Widget::Renderer::Panel * const MainWindow::getRender() const
	{
		return getPanel<QT::Widget::Renderer::Panel>( "RENDER_WINDOW_KEY" );
	}

	void		MainWindow::refreshWindowTitle() { setWindowTitle( QString::fromStdString( _getWindowTitle() ) ); }
	std::string MainWindow::_getWindowTitle() const
	{
		std::string title = "VTX (BETA)";
#ifndef VTX_PRODUCTION
		title += " - DEV";
#ifdef _DEBUG
		title += " - DEBUG";
#else
		title += " - RELEASE";
#endif
#endif

		return title;
	}

	void MainWindow::addDockWidgetAsTabified(
		QDockWidget * const		 p_dockWidget,
		const Qt::DockWidgetArea p_area,
		const Qt::Orientation	 p_orientation,
		const bool				 p_visible
	)
	{
		addDockWidget( p_area, p_dockWidget, p_orientation );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}

	void MainWindow::addDockWidgetAsTabified(
		QDockWidget * const p_dockWidget,
		QDockWidget * const p_neighbour,
		Qt::Orientation		p_orientation,
		const bool			p_visible
	)
	{
		splitDockWidget( p_neighbour, p_dockWidget, p_orientation );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}
	void MainWindow::addDockWidgetAsFloating(
		QDockWidget * const p_dockWidget,
		const QSize &		p_size,
		const bool			p_visible
	)
	{
		// Create an emplacement for the widget before setting it floating to prevent warning
		// TODO check https://bugreports.qt.io/browse/QTBUG-88157 to remove useless tabifyDockWidget
		// tabifyDockWidget( _inspectorWidget, p_dockWidget );

		p_dockWidget->setFloating( true );
		p_dockWidget->resize( p_size );

		if ( !p_dockWidget->isVisible() && p_visible )
			p_dockWidget->show();
		else if ( p_dockWidget->isVisible() && !p_visible )
			p_dockWidget->hide();
	}

	void MainWindow::addFloatingWindow( QDialog * const p_window, const QSize & p_size, const bool p_visible )
	{
		p_window->resize( p_size );

		if ( !p_window->isVisible() && p_visible )
			p_window->show();
		else if ( p_window->isVisible() && !p_visible )
			p_window->hide();
	}

	UI::Core::WindowMode MainWindow::_getWindowModeFromWindowState( const Qt::WindowStates & p_state )
	{
		UI::Core::WindowMode res;

		if ( p_state & Qt::WindowState::WindowFullScreen )
			res = UI::Core::WindowMode::Fullscreen;
		else if ( p_state & Qt::WindowState::WindowMaximized )
			res = UI::Core::WindowMode::Maximized;
		else if ( p_state & Qt::WindowState::WindowMinimized )
			res = UI::Core::WindowMode::Minimized;
		else
			res = UI::Core::WindowMode::Windowed;

		return res;
	}

	void MainWindow::keyPressEvent( QKeyEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleKeyboardEvent( *p_event );
	}
	void MainWindow::keyReleaseEvent( QKeyEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleKeyboardEvent( *p_event );
	}
	void MainWindow::mousePressEvent( QMouseEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleMouseEvent( *p_event );
	}
	void MainWindow::mouseReleaseEvent( QMouseEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleMouseEvent( *p_event );
	}
	void MainWindow::mouseDoubleClickEvent( QMouseEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleMouseEvent( *p_event );
	}
	void MainWindow::mouseMoveEvent( QMouseEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleMouseEvent( *p_event );
	}
	void MainWindow::wheelEvent( QWheelEvent * const p_event )
	{
		// App::INPUT_MANAGER().handleMouseWheelEvent( *p_event );
	}

	void MainWindow::changeEvent( QEvent * const p_event )
	{
		QMainWindow::changeEvent( p_event );

		if ( p_event->type() == QEvent::ActivationChange && this->isActiveWindow() )
		{
			App::INPUT_MANAGER().clearKeyboardBuffer();
		}
	}
	void MainWindow::closeEvent( QCloseEvent * const p_closeEvent )
	{
		// if ( App::SCENE().hasAnyModifications() )
		//{
		//	p_closeEvent->ignore();
		//	Worker::CallbackThread callback = Worker::CallbackThread(
		//		[]( const uint p_code )
		//		{
		//			if ( p_code )
		//				VTXApp::get().quit();
		//		}
		//	);

		//	UI::Dialog::leavingSessionDialog( callback );
		//}
		// else
		{
			p_closeEvent->accept();
		}
	}

	void MainWindow::_updatePicker() const
	{
		// const QT::State::Visualization * const visualizationState
		//	= QT_APP()->getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		// const App::Old::VTX_ID & pickerID = visualizationState->getCurrentPickerID();

		// if ( pickerID == ID::Controller::PICKER )
		//{
		//	_cursorHandler->applyCursor(
		//		CursorHandler::Cursor::DEFAULT, &getWidget( UI::ID::Window::RENDER ), "Picker" );
		// }
		// else if ( pickerID == ID::Controller::MEASUREMENT )
		//{
		//	const Controller::MeasurementPicker * const measurementPicker
		//		= visualizationState->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

		//	CursorHandler::Cursor cursor;

		//	switch ( measurementPicker->getCurrentMode() )
		//	{
		//	case Controller::MeasurementPicker::Mode::DISTANCE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DISTANCE;
		//		break;
		//	case Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DISTANCE_TO_CYCLE;
		//		break;
		//	case Controller::MeasurementPicker::Mode::ANGLE: cursor = CursorHandler::Cursor::MEASUREMENT_ANGLE; break;
		//	case Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE:
		//		cursor = CursorHandler::Cursor::MEASUREMENT_DIHEDRAL_ANGLE;
		//		break;
		//	default: cursor = CursorHandler::Cursor::DEFAULT; break;
		//	}

		//	_cursorHandler->applyCursor( cursor, &getWidget( UI::ID::Window::RENDER ), "Picker_Measurement" );
		//}
	}

	void MainWindow::_checkUnknownFloatableWindows()
	{
		//_checkUnknownFloatableWindow( _settingWidget, Style::SETTINGS_PREFERRED_SIZE );
		//_checkUnknownFloatableWindow( _structuralAlignmentWidget, Style::STRUCTURAL_ALIGNMENT_PREFERRED_SIZE );
	}
	void MainWindow::_checkUnknownFloatableWindow( QDockWidget * const p_widget, const QSize & p_defaultSize )
	{
		if ( p_widget->widget()->size().height() == QT_UNKNOWN_WIDGET_DEFAULT_LAYOUT_HEIGHT )
		{
			addDockWidgetAsFloating( p_widget, p_defaultSize, p_widget->isVisible() );
		}
	}
} // namespace VTX::UI::QT
