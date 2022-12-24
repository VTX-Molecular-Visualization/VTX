#include "application_qt.hpp"
#include "action/action_manager.hpp"
#include "dialog.hpp"
#include "generic/base_opengl.hpp"
#include "src/action/main.hpp"
#include "vtx_app.hpp"
#include <QCoreApplication>
#include <QSurfaceFormat>
#include <exception>

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

		VTX_ACTION( new VTX::Action::Main::OpenApi( "1aga" ) );

		_returnCode = exec();
	}

	void ApplicationQt::update()
	{
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		Core::BaseUIApplication::update();
		_stateMachine->update( elapsed );
	}

	void ApplicationQt::quit() { closeAllWindows(); };

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
		_mainWindow = new UI::QT::MainWindow();
		_mainWindow->setupUi();
		_mainWindow->show();

		_mainWindow->initWindowLayout();
	}

	void ApplicationQt::_postInit( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::_postInit( p_args );

		if ( !_mainWindow->isOpenGLValid() )
		{
			UI::QT::Dialog::openGLInitializationFail();
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
