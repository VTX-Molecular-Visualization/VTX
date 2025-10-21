#include "ui/qt/application.hpp"
#include "app/services.hpp"
#include "ui/qt/macros.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/services.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QStyle>
#include <app/ecs.hpp>
#include <app/infos.hpp>
#include <util/event_hub.hpp>

VTX_INIT_RESOURCES( vtx_qt_resources_ui )

namespace VTX::UI::QT
{

	int zero = 0;
	Application::Application( const App::Args & p_args ) : App::VTXApp( p_args ), QApplication( zero, nullptr )

	{
		using namespace Resources;
		using namespace VTX::App;

		// Application info.
		const std::string version = std::to_string( VERSION_MAJOR ) + "." + std::to_string( VERSION_MINOR ) + "."
									+ std::to_string( VERSION_PATCH );
		std::string displayName = APPLICATION_DISPLAY_NAME.data() + std::string( " - v" ) + version;
		if ( p_args.has( App::ARG_DEBUG ) )
		{
			displayName += " (Debug)";
		}

		setWindowIcon( QIcon( SPRITE_LOGO.data() ) );
		setApplicationDisplayName( QString::fromStdString( displayName ) );
		setApplicationName( QString::fromStdString( APPLICATION_NAME.data() ) );
		setApplicationVersion( QString::fromStdString( version ) );
		setOrganizationName( QString::fromStdString( ORGANIZATION_NAME.data() ) );
		setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN.data() ) );

		// Settings.
		App::ECS::setCtx<Settings>();

		// Create main window.
		_mainWindow = new Widget::MainWindow();
		// Store in context.
		App::ECS::setCtx<QPointer<Widget::MainWindow>>( _mainWindow );

		// Load theme and restore settings.
		try
		{
			_loadTheme();
			SETTINGS().restore();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to restore settings: {}", e.what() );
		}

		// Show.
		_mainWindow->show();

		// Connect quit event that can come from VTXApp.
		App::HUB().connect<App::Events::ApplicationStopped, &Application::stop>( this );

		// Save settings just before quitting (after QCoreApplication::quit).
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				VTX_TRACE( "QCoreApplication::aboutToQuit" );
				try
				{
					SETTINGS().save();
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "Failed to save settings: {}", e.what() );
				}
			}
		);

		// Run the main loop.
		connect(
			&_timer,
			&QTimer::timeout,
			[ this ] { VTXApp::update( _durationTimer.intervalTime(), _durationTimer.elapsedTime() ); }
		);
		_timer.start( 0 );
		_durationTimer.start();
	}

	Application::~Application()
	{
		App::ECS::removeCtx<QPointer<Widget::MainWindow>>();
		_mainWindow.clear();
	}

	void Application::start()
	{
		// Run Qt main loop.
		exec();
		VTX_TRACE( "Application::start(): Qt loop exited" );
		_timer.stop();
	}

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

		QPalette lightPalette = QApplication::style()->standardPalette();

		setPalette( p );
	}

	void Application::stop()
	{
		VTX_TRACE( "Application::stop()" );
		// Safely quit Qt main loop.
		QTimer::singleShot( 0, this, &QCoreApplication::quit );
	}

} // namespace VTX::UI::QT
