#include "qt/application.hpp"
#include "qt/main_window.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <app/infos.hpp>
#include <util/enum.hpp>

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
	Application::Application() : UI::BaseApplication<MainWindow>(), QApplication( zero, nullptr )

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

		_loadTheme();
	}

	Application::~Application() {}

	/*
	bool Application::event( QEvent * event )
	{
		// Handle quit event.
		if ( event->type() == QEvent::Close )
		{
			App::VTX_ACTION().execute<App::Action::Application::Quit>();
			QApplication::quit();
			return true;
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
			//_restoreSettings();
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
			_timer.stop();
			_elapsedTimer.invalidate();
			_mainWindow->close();
			QApplication::quit();
		};

		// Connect quit action.
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				try
				{
					_saveSettings();
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Failed to save settings: {}", e.what() );
				}
			}
		);

		// Run main loop.
		connect( &_timer, &QTimer::timeout, [ this ] { VTXApp::update( _elapsedTimer.elapsed() ); } );
		_timer.start( 0 );
		_elapsedTimer.start();

		// Then block.
		exec();
	}

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

	void Application::_saveSettings()
	{
		VTX_INFO( "Saving settings: {}", SETTINGS.fileName().toStdString() );
		SETTINGS.setValue( "geometry", _mainWindow->saveGeometry() );
		SETTINGS.setValue( "windowState", _mainWindow->saveState() );

		if ( SETTINGS.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( SETTINGS.status() ) ) );
		}

		SETTINGS.sync();
	}

	void Application::_restoreSettings()
	{
		if ( SETTINGS.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( SETTINGS.status() ) ) );
		}

		VTX_INFO( "Restoring settings: {}", SETTINGS.fileName().toStdString() );
		_mainWindow->restoreGeometry( SETTINGS.value( "geometry" ).toByteArray() );
		_mainWindow->restoreState( SETTINGS.value( "windowState" ).toByteArray() );
	}

} // namespace VTX::UI::QT
