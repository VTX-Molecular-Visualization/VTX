#include "qt/application.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
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

		VTX_INFO( "Show splashscreen" );
		//_qSplashScreen = new QSplashScreen( QPixmap( SPRITE_SPLASH ) );
		//_qSplashScreen->show();
		//_qSplashScreen->showMessage( "Loading..." );

		setWindowIcon( QIcon( SPRITE_LOGO ) );
		setApplicationDisplayName( QString::fromStdString( APPLICATION_DISPLAY_NAME ) );
		setApplicationName( QString::fromStdString( APPLICATION_NAME ) );
		setApplicationVersion( QString::fromStdString( APPLICATION_VERSION ) );
		setOrganizationName( QString::fromStdString( ORGANIZATION_NAME ) );
		setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN ) );
		// setQuitOnLastWindowClosed( false );

		_loadTheme();
	}

	Application::~Application() {}

	/*
	bool Application::event( QEvent * p_event )
	{
		// Handle quit event.

		if ( p_event->type() == QEvent::Close )
		{
			VTX_DEBUG( "Qt application close event" );
			// App::VTX_ACTION().execute<App::Action::Application::Quit>();
			// QApplication::quit();
			// return true;
		}

		return false;
	}
*/

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
			VTX_DEBUG( "Qt stop callback" );
			_mainWindow->close();
		};

		// Connect quit action.
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				VTX_DEBUG( "QCoreApplication::aboutToQuit" );

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
			[ this ]
			{
				static const float S_TO_MS = 1e3;
				VTXApp::update( _deltaTimer.intervalTime() * S_TO_MS, _elapsedTimer.elapsedTime() * S_TO_MS );
			}
		);
		_timer.start( 0 );
		_elapsedTimer.start();
		_deltaTimer.start();

		// Then block to run Qt events loop.
		exec();
		VTX_DEBUG( "Qt loop exited" );
		_timer.stop();
	}

	void Application::save() {}

	void Application::restore() {}

	void Application::_loadTheme()
	{
		using namespace Resources;

		// Load main stylesheet.
		QFile stylesheetFile( FILE_STYLESHEET );
		stylesheetFile.open( QFile::ReadOnly );
		QString stylesheet = stylesheetFile.readAll();

		// Load os-specific stylesheet.
#if _WIN32
		QFile stylesheetOSFile( FILE_STYLESHEET_WINOWS );
#elif __linux__
		QFile stylesheetOSFile( FILE_STYLESHEET_LINUX );
#elif __APPLE__
		QFile stylesheetOSFile( FILE_STYLESHEET_MACOS );
#else
		QFile stylesheetOSFile();
		assert( true );
#endif

		stylesheetOSFile.open( QFile::ReadOnly );
		stylesheet += '\n' + stylesheetOSFile.readAll();

		// TODO: Load theme file and apply to stylesheet.

		// Set stylesheet to app.
		//_qApplication->setStyleSheet( stylesheet );
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
