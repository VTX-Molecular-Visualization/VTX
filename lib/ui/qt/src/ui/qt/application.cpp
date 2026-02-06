#include "ui/qt/application.hpp"
#include "ui/qt/dialog/trajectory_association.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <app/ecs.hpp>
#include <app/infos.hpp>
#include <app/services.hpp>
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

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
		try
		{
			App::ECS::setCtx<Settings>();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Failed to restore settings: {}", p_e.what() );
		}

		// Load theme.
		App::ECS::setCtx<Style>().load( _tools );

		// Create and show main window.
		App::ECS::setCtx<Widget::MainWindow>().show();

		// Connect quit event that can come from VTXApp.
		App::HUB().connect<App::Events::ApplicationStop, &Application::stop>( this );
		App::HUB().connect<App::Events::TrajectoryFileAssociation, &Application::_trajectoryFileAssociation>( this );

		// After quit, last loop.
		connect(
			this,
			&QCoreApplication::aboutToQuit,
			[ this ]
			{
				VTX_TRACE( "QCoreApplication::aboutToQuit" );
				// Properly destroy graphics resources before Qt kill the context.
				App::RENDERER().clear();
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
		// Delete main window and all its widgets.
		App::ECS::removeCtx<Widget::MainWindow>();

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
		VTXApp::start();

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
	void Application::_trajectoryFileAssociation( const App::Events::TrajectoryFileAssociation & p_event ) noexcept
	{
		auto e = new Dialog::TrajectoryAssociation( p_event.path );
		e->exec();
	}

} // namespace VTX::UI::QT
