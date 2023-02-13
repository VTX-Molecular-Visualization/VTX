#include "application_qt.hpp"
// #include "dialog.hpp"
#include "qt/main_window.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/tool/render/dialog.hpp"
#include "widget_factory.hpp"
#include <QCoreApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QPalette>
#include <QSurfaceFormat>
// #include <exception>
#include <old/tool/logger.hpp>
// #include <old/action/action_manager.hpp>
// #include <old/action/main.hpp>
#include <old/generic/base_opengl.hpp>
// #include <old/vtx_app.hpp>

namespace VTX::UI::QT
{
	void ApplicationQt::configure()
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
		QSurfaceFormat format;

		format.setVersion( OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		// format.setSwapInterval( int( VTX_SETTING().ACTIVE_VSYNC_DEFAULT ) );
		format.setSwapInterval( int( true ) );

		QSurfaceFormat::setDefaultFormat( format );

		// Init resources
		Q_INIT_RESOURCE( resources_ui );
	}

	int ZERO = 0;
	ApplicationQt::ApplicationQt() : Core::BaseUIApplication(), QApplication( ZERO, nullptr )
	{
		connect( this, &QCoreApplication::aboutToQuit, this, &ApplicationQt::stop );
	}
	ApplicationQt::~ApplicationQt() {}

	void ApplicationQt::init() { Core::BaseUIApplication::init(); }
	void ApplicationQt::start( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::start( p_args );

		_handleArgs( p_args );

		_returnCode = exec();
	}

	void ApplicationQt::update()
	{
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		Core::BaseUIApplication::update();
		_stateMachine->update( elapsed );
	}

	void ApplicationQt::quit() { QApplication::quit(); };

	void ApplicationQt::_initUI( const std::vector<std::string> & p_args )
	{
		// Create statemachine.
		_stateMachine = new State::StateMachine();
		goToState( ID::State::VISUALIZATION );

		// Create UI.
		_initQt();

		_instantiateMainWindow();

		_mainWindow->getMainMenu().setCurrentTab( 0 );
		//_renderWidget->setFocus();

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &ApplicationQt::update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();
	}

	void ApplicationQt::_initQt()
	{
		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
	}

	void ApplicationQt::_instantiateMainWindow()
	{
		_mainWindow = WidgetFactory::get().instantiateWidget<QT::MainWindow>( nullptr, "MainWindow" );
	}

	void ApplicationQt::_postInit( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::_postInit( p_args );

		_mainWindow->show();
		_mainWindow->initWindowLayout();

		if ( !_mainWindow->isOpenGLValid() )
		{
			UI::QT::Tool::Render::Dialog::openGLInitializationFail();
			return;
		}
	}

	void ApplicationQt::renderScene() const
	{
		if ( VTX_SETTING().getActivateRenderer() && VTXApp::get().MASK )
		{
			_mainWindow->updateRender();
		}
	}

	void ApplicationQt::stop()
	{
		_mainWindow->saveLayout();

		if ( _stateMachine != nullptr )
		{
			delete _stateMachine;
		}

		// TODO BaseUIApplication::stop() called here because some model are strongly linked to _gl
		BaseUIApplication::stop();

		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}
	}

	void ApplicationQt::goToState( const std::string & p_name, void * const p_arg )
	{
		try
		{
			_stateMachine->goToState( p_name, p_arg );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
		}
	}

	bool ApplicationQt::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
#ifdef VTX_PRODUCTION
			UI::Dialog::unhandledException();
#endif
			return true;
		}
	}

} // namespace VTX::UI::QT