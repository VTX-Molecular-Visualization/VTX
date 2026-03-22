#include "ui/qt/application.hpp"
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
	Application::Application( const App::Args & p_args ) : QApplication( zero, nullptr ), _app( p_args )
	{
		using namespace Resources;
		using namespace App;

		_splashScreen = new QSplashScreen( QPixmap( Resources::SPRITE_SPLASH.data() ) );
		_splashScreen->show();

		// Application info.
		const std::string version = std::to_string( VERSION_MAJOR ) + "." + std::to_string( VERSION_MINOR ) + "."
									+ std::to_string( VERSION_PATCH );
		std::string displayName = APPLICATION_DISPLAY_NAME.data() + std::string( " v" ) + version;
		if ( p_args.has( App::ARG_DEBUG ) )
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
			App::ECS::setCtx<SelectionManager>( this );
			App::ECS::setCtx<Widget::MainWindow>();

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

	QAction * const Application::_getOrCreateAction( const App::UI::DescAction & p_action )
	{
		const std::string_view key = p_action.key.empty() ? VTX::Util::typeName<App::UI::DescAction>() : p_action.key;

		// Find existing action.
		QAction * qAction = Q_APP()->findChild<QAction *>( key );

		if ( qAction )
		{
			return qAction;
		}
		else
		{
			qAction = new QAction( Q_APP() );
			qAction->setObjectName( key );

			VTX_TRACE( "UI action created: {}", key );

			// Name.
			qAction->setText( QString::fromStdString( p_action.name ) );
			// Group.
			if ( p_action.group )
			{
				auto * qActionGroup = Q_APP()->findChild<QActionGroup *>( *p_action.group );
				if ( not qActionGroup )
				{
					qActionGroup = new QActionGroup( Q_APP() );
					qActionGroup->setObjectName( *p_action.group );
				}

				qAction->setCheckable( true );
				qActionGroup->addAction( qAction );
			}
			if ( p_action.group && *p_action.group == "Theme" )
			{
				if ( p_action.key == VTX::Util::typeName<Action::Theme::System>()
					 && STYLE().getCurrentTheme() == Style::E_THEME::SYSTEM )
				{
					qAction->setChecked( true );
				}
				else if ( p_action.key == VTX::Util::typeName<Action::Theme::Light>()
						  && STYLE().getCurrentTheme() == Style::E_THEME::LIGHT )
				{
					qAction->setChecked( true );
				}
				else if ( p_action.key == VTX::Util::typeName<Action::Theme::Dark>()
						  && STYLE().getCurrentTheme() == Style::E_THEME::DARK )
				{
					qAction->setChecked( true );
				}
			}
			// Tip.
			if ( p_action.tip )
			{
				QString tip = QString::fromStdString( *p_action.tip );

				if ( p_action.shortcut )
				{
					tip.append( " (" + *p_action.shortcut + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( p_action.icon )
			{
				if ( std::holds_alternative<int>( *p_action.icon ) )
				{
					QIcon icon = STYLE().iconFromCodepoint( std::get<int>( *p_action.icon ) );
					qAction->setIcon( icon );
				}
				else if ( std::holds_alternative<std::string>( *p_action.icon ) )
				{
					qAction->setIcon(
						QIcon( QString::fromStdString( ( ":/" + std::get<std::string>( *p_action.icon ) ) ) )
					);
				}
				else
				{
					VTX_ERROR( "Invalid icon type for action: {}", key );
				}
			}
			// Shortcut.
			if ( p_action.shortcut )
			{
				qAction->setShortcut( QKeySequence( QString::fromStdString( *p_action.shortcut ) ) );
			}
			// Action.
			if ( p_action.trigger )
			{
				QObject::connect( qAction, &QAction::triggered, *p_action.trigger );
			}
		}

		return qAction;
	}

} // namespace VTX::UI::QT
