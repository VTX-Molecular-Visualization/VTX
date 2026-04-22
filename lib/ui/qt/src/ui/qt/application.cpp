#include "ui/qt/application.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/dialog/trajectory_association.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <app/constants.hpp>
#include <app/ecs.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <renderer/renderer.hpp>
#include <ui/qt/actions.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT
{

	int zero = 0;
	Application::Application( App::Arguments && p_args ) : QApplication( zero, nullptr ), _app( std::move( p_args ) )
	{
		using namespace Resources;
		using namespace App;

		_splashScreen = new QSplashScreen( QPixmap( Resources::SPRITE_SPLASH.data() ) );
		_splashScreen->show();

		// Application info.
		const std::string version	  = std::to_string( VERSION_MAJOR ) + "." + std::to_string( VERSION_MINOR ) + "."
										+ std::to_string( VERSION_PATCH );
		std::string		  displayName = APPLICATION_DISPLAY_NAME.data() + std::string( " " ) + version + ( "-beta" );
		if ( ARGS().debug )
		{
			displayName += " (Debug)";
		}

		setWindowIcon( QIcon( QPixmap( SPRITE_LOGO.data() ) ) );
		setApplicationDisplayName( QString::fromStdString( displayName ) );
		setApplicationName( QString::fromStdString( APPLICATION_NAME.data() ) );
		setApplicationVersion( QString::fromStdString( version ) );
		setOrganizationName( QString::fromStdString( ORGANIZATION_NAME.data() ) );
		setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN.data() ) );

		// Connect quit.
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				VTX_TRACE( "QCoreApplication::aboutToQuit" );
				try
				{
					// Properly destroy graphics resources before Qt kill native surfaces.
					App::RENDERER().clear();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Exception during renderer cleanup: {}", p_e.what() );
				}
				catch ( ... )
				{
					VTX_ERROR( "Unknown exception during renderer cleanup" );
				}
			}
		);

		// Connect the main loop.
		connect(
			&_timer,
			&QTimer::timeout,
			[ this ] { _app.update( _durationTimer.intervalTime(), _durationTimer.elapsedTime() ); }
		);
	}

	Application::~Application()
	{
		// Delete elements that need SETTING().
		try
		{
			App::ECS::removeCtx<Widget::MainWindow>();
			App::ECS::removeCtx<ActionRegistry>();
			App::ECS::removeCtx<Style::StyleManager>();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Exception during UI cleanup: {}", p_e.what() );
		}
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception during UI cleanup" );
		}

		// Save settings on disk.
		try
		{
			SETTINGS().save();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to save settings: {}", e.what() );
		}
	}

	void Application::start()
	{
		QTimer::singleShot( 0, this, &Application::_postQtStartup );

		// Run Qt main loop.
		exec();
		VTX_TRACE( "Application::start(): Qt loop exited" );
		_timer.stop();
	}

	void Application::stop()
	{
		VTX_TRACE( "Application::stop()" );
		// Safely quit Qt main loop.
		QTimer::singleShot( 0, this, &QCoreApplication::quit );
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
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception in Qt event handler" );
			return true;
		}
	}

	void Application::_postQtStartup()
	{
		try
		{
			_app.startServices();

			try
			{
				App::ECS::setCtx<Settings>();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Failed to restore settings: {}", p_e.what() );
			}

			App::ECS::setCtx<Style::StyleManager>();
			App::ECS::setCtx<ActionRegistry>();
			App::ECS::setCtx<SelectionManager>( this );

			Action::registerActions( UI_ACTIONS() );
			for ( auto & tool : _app.getTools() )
			{
				assert( tool != nullptr );
				tool->registerActions();
			}

			App::ECS::setCtx<Widget::MainWindow>();

			UI_ACTIONS().installShortcuts( MAIN_WINDOW() );

			for ( auto & tool : _app.getTools() )
			{
				assert( tool != nullptr );
				tool->buildUI();
			}

			App::HUB().connect<App::Events::ApplicationStop, &Application::stop>( this );
			_app.createInitialEntities();

			try
			{
				Renderer::Desc::NativeContextInfo contextInfo;
				contextInfo.surface = MAIN_WINDOW().getNativeSurface();
				contextInfo.display = MAIN_WINDOW().getNativeDisplay();
				contextInfo.plateform
					= static_cast<Renderer::Desc::E_NATIVE_PLATEFORM>( MAIN_WINDOW().getNativePlatform() );

				App::RENDERER().setOpenGL( contextInfo, App::SESSION().getShadersDir() );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( p_e.what() );
				App::RENDERER().setDefault();
				App::HUB().trigger<App::Events::ApplicationError>(
					"Unable to create OpenGL context. Update your drivers and check your hardware "
					"compatibility."
				);
			}
			_app.finishStartup();
			_app.handleArgs();

			MAIN_WINDOW().show();
			_splashScreen->finish( &MAIN_WINDOW() );

			_timer.start( 0 );
			_durationTimer.start();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Startup failed: {}", p_e.what() );
			QCoreApplication::quit();
		}
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception during startup" );
			QCoreApplication::quit();
		}
	}
} // namespace VTX::UI::QT
