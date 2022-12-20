#include "application.hpp"
#include "dialog.hpp"
#include "generic/base_opengl.hpp"
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
	ApplicationQt::ApplicationQt() : Core::Application(), QApplication( ZERO, nullptr )
	{
		connect( this, &QCoreApplication::aboutToQuit, this, &ApplicationQt::stop );
	}
	ApplicationQt::~ApplicationQt() {}

	void ApplicationQt::init() { Core::Application::init(); }
	void ApplicationQt::start( const std::vector<std::string> & p_args )
	{
		Core::Application::start( p_args );

		_handleArgs( p_args );

		_returnCode = exec();
	}

	void ApplicationQt::update() { Core::Application::update(); }

	void ApplicationQt::quit() { closeAllWindows(); };

	void ApplicationQt::_initUI( const std::vector<std::string> & p_args )
	{
		//// Create statemachine.
		//_stateMachine = new State::StateMachine();
		// goToState( ID::State::VISUALIZATION );

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

		if ( !_mainWindow->isOpenGLValid() )
		{
			UI::QT::Dialog::openGLInitializationFail();
			return;
		}
	}

	void ApplicationQt::stop()
	{
		_mainWindow->saveLayout();

		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}
		// if ( _stateMachine != nullptr )
		//{
		//	delete _stateMachine;
		// }

		Application::stop();
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
