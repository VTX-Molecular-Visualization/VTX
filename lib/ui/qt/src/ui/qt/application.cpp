#include "ui/qt/application.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <app/infos.hpp>

namespace VTX::UI::QT
{
	// Need to be called before QApplication constructor.
	void Application::configure()
	{
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
	}

	// Create QApplication with zero argc and nullptr argv.
	int zero = 0;
	Application::Application() : App::UI::BaseApplication<Widget::MainWindow>(), QApplication( zero, nullptr )

	{
		using namespace Resources;
		using namespace VTX::App::Info;
		//_qSplashScreen = new QSplashScreen( QPixmap( SPRITE_SPLASH ) );
		//_qSplashScreen->show();
		//_qSplashScreen->showMessage( "Loading..." );

		setWindowIcon( QIcon( SPRITE_LOGO.data() ) );
		setApplicationDisplayName( QString::fromStdString( APPLICATION_DISPLAY_NAME.data() ) );
		setApplicationName( QString::fromStdString( APPLICATION_NAME.data() ) );
		setApplicationVersion( QString::fromStdString( APPLICATION_VERSION.data() ) );
		setOrganizationName( QString::fromStdString( ORGANIZATION_NAME.data() ) );
		setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN.data() ) );
		// setQuitOnLastWindowClosed( false );
	}

	Application::~Application() {}

	bool Application::notify( QObject * const p_receiver, QEvent * const p_event )
	{
		try
		{
			return QApplication::notify( p_receiver, p_event );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
			return true;
		}
	}

	void Application::_start()
	{
		try
		{
			_loadTheme();
			// Restore settings after main window is built.
			SETTINGS.restore();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to restore settings: {}", e.what() );
		}

		// Show.
		//_qSplashScreen->finish( _mainWindow.get() );
		_mainWindow->show();

		// On quit.
		onStop += [ this ]
		{
			VTX_TRACE( "Qt stop callback" );
			_mainWindow->setClosing( true );
			QApplication::quit();
		};

		// Connect quit action.
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				VTX_TRACE( "QCoreApplication::aboutToQuit" );

				try
				{
					SETTINGS.save();
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Failed to save settings: {}", e.what() );
				}
			}
		);

		// Run main loop.
		connect(
			&_timer,
			&QTimer::timeout,
			[ this ] { VTXApp::update( _deltaTimer.intervalTime(), _elapsedTimer.elapsedTime() ); }
		);
		_timer.start( 0 );
		//_timer.setTimerType( Qt::PreciseTimer );
		_elapsedTimer.start();
		_deltaTimer.start();

		// Then block to run Qt events loop.
		exec();
		VTX_TRACE( "Qt loop exited" );
		_timer.stop();
	}

	void Application::_loadTheme()
	{
		using namespace Resources;

		// Load main stylesheet.
		QFile stylesheetFile( FILE_STYLESHEET.data() );
		stylesheetFile.open( QFile::ReadOnly );
		QString stylesheet = stylesheetFile.readAll();

		// Load os-specific stylesheet.
#if _WIN32
		QFile stylesheetOSFile( FILE_STYLESHEET_WINDOWS.data() );
#elif __linux__
		QFile stylesheetOSFile( FILE_STYLESHEET_LINUX.data() );
#elif __APPLE__
		QFile stylesheetOSFile( FILE_STYLESHEET_MACOS.data() );
#else
		QFile stylesheetOSFile();
		assert( true );
#endif

		stylesheetOSFile.open( QFile::ReadOnly );
		stylesheet += '\n' + stylesheetOSFile.readAll();

		// TODO: move to super class?
		for ( const App::Tool::BaseTool * const tool : _tools )
		{
			if ( tool->getStyle().has_value() )
			{
				stylesheet += '\n' + tool->getStyle().value();
			}
		}

		// Set stylesheet to app.
		setStyleSheet( stylesheet );
		setStyle( "fusion" );

		QPalette p = palette();

		/*
		palette.setBrush( QPalette::Window, Qt::Dense3Pattern );
		palette.setBrush( QPalette::WindowText, Qt::white );
		palette.setBrush( QPalette::Base, QColor( 25, 25, 25 ) );
		palette.setBrush( QPalette::AlternateBase, QColor( 53, 53, 53 ) );
		palette.setBrush( QPalette::ToolTipBase, Qt::white );
		palette.setBrush( QPalette::ToolTipText, Qt::white );
		palette.setBrush( QPalette::Text, Qt::white );
		palette.setBrush( QPalette::Button, QColor( 53, 53, 53 ) );
		palette.setBrush( QPalette::ButtonText, Qt::white );
		palette.setBrush( QPalette::BrightText, Qt::red );
		 palette.setColor(QPalette::Link, QColor(0, 0, 255));
		palette.setBrush( QPalette::Highlight, QColor( 142, 45, 197 ).lighter() );
		palette.setBrush( QPalette::HighlightedText, Qt::black );
		*/

		setPalette( p );
	}

} // namespace VTX::UI::QT
