#include "ui/qt/main_window.hpp"
#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/util.hpp"
#include "ui/qt/widget_factory.hpp"
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

		_mainMenuBar = WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
		setMenuBar( _mainMenuBar );

		setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

		_loadStyleSheet( ":/stylesheet_ui.css" );
	}

	void MainWindow::initWindowLayout() {}

	QT::Widget::MainMenu::MenuTooltabWidget & MainWindow::getMainMenuToolTab( const Core::ToolLayoutData & layoutData )
	{
		return dynamic_cast<QT::Widget::MainMenu::MenuTooltabWidget &>( getMainMenu().getTab( layoutData.tabName ) );
	}
	QT::Widget::MainMenu::MenuToolBlockWidget & MainWindow::getMainMenuToolBlock(
		const Core::ToolLayoutData & layoutData
	)
	{
		return dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
			getMainMenu().getTab( layoutData.tabName ).getToolBlock( layoutData.blockName )
		);
	}

	void MainWindow::addShortcut( const std::string & p_shortcut, QAction * const p_action )
	{
		assert( _shortcuts.find( p_shortcut ) == _shortcuts.end() );

		p_action->setParent( this );

		connect(
			new QShortcut( QKeySequence( tr( p_shortcut.c_str() ) ), this ),
			&QShortcut::activated,
			p_action,
			&QAction::trigger
		);

		_shortcuts.emplace( p_shortcut );
	}

	void MainWindow::_loadStyleSheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		const QString stylesheet = stylesheetFile.readAll();
		setStyleSheet( stylesheet );
	}

	void MainWindow::appendStylesheet( const char * p_stylesheetPath )
	{
		QFile stylesheetFile( p_stylesheetPath );
		stylesheetFile.open( QFile::ReadOnly );

		const QString stylesheetContent = stylesheetFile.readAll();
		const QString newStylesheet		= styleSheet() + '\n' + stylesheetContent;

		setStyleSheet( newStylesheet );
	}

	void MainWindow::_setupSlots() {}
	void MainWindow::localize() { setWindowTitle( "VTX" ); }

	QT::Tool::Render::Widget::RenderWidget * MainWindow::getRender()
	{
		return getPanel<QT::Tool::Render::Widget::RenderWidget>( "RENDER_WINDOW_KEY" );
	}
	const QT::Tool::Render::Widget::RenderWidget * const MainWindow::getRender() const
	{
		return getPanel<QT::Tool::Render::Widget::RenderWidget>( "RENDER_WINDOW_KEY" );
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

	Core::WindowMode MainWindow::_getWindowModeFromWindowState( const Qt::WindowStates & p_state )
	{
		Core::WindowMode res;

		if ( p_state & Qt::WindowState::WindowFullScreen )
			res = Core::WindowMode::Fullscreen;
		else if ( p_state & Qt::WindowState::WindowMaximized )
			res = Core::WindowMode::Maximized;
		else if ( p_state & Qt::WindowState::WindowMinimized )
			res = Core::WindowMode::Minimized;
		else
			res = Core::WindowMode::Windowed;

		return res;
	}

	void MainWindow::keyPressEvent( QKeyEvent * const p_event ) { INPUT_MANAGER().handleKeyboardEvent( *p_event ); }
	void MainWindow::keyReleaseEvent( QKeyEvent * const p_event ) { INPUT_MANAGER().handleKeyboardEvent( *p_event ); }
	void MainWindow::mousePressEvent( QMouseEvent * const p_event ) { INPUT_MANAGER().handleMouseEvent( *p_event ); }
	void MainWindow::mouseReleaseEvent( QMouseEvent * const p_event ) { INPUT_MANAGER().handleMouseEvent( *p_event ); }
	void MainWindow::mouseDoubleClickEvent( QMouseEvent * const p_event )
	{
		INPUT_MANAGER().handleMouseEvent( *p_event );
	}
	void MainWindow::mouseMoveEvent( QMouseEvent * const p_event ) { INPUT_MANAGER().handleMouseEvent( *p_event ); }
	void MainWindow::wheelEvent( QWheelEvent * const p_event ) { INPUT_MANAGER().handleMouseWheelEvent( *p_event ); }

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
