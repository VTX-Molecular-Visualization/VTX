#include "ui/qt/application.hpp"
#include "app/services.hpp"
#include "ui/qt/macros.hpp"
#include "ui/qt/menu/file.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QAction>
#include <QActionGroup>
#include <QApplication>
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
		try
		{
			App::ECS::setCtx<Settings>();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to restore settings: {}", e.what() );
		}

		// Create model.
		auto & model = App::ECS::setCtx<Model>();
		App::ECS::setCtx<SelectionModel>( &model );

		// Load theme.
		App::ECS::setCtx<Style>().load( _tools );

		// Create and show main window.
		App::ECS::setCtx<Widget::MainWindow>().show();

		// Connect quit event that can come from VTXApp.
		App::HUB().connect<App::Events::ApplicationStop, &Application::stop>( this );

		// After quit, last loop.
		connect(
			this, &QCoreApplication::aboutToQuit, [ this ] { VTX_TRACE( "QCoreApplication::aboutToQuit" ); }

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
			if ( p_action.group.has_value() )
			{
				auto * qActionGroup = Q_APP()->findChild<QActionGroup *>( p_action.group.value() );
				if ( not qActionGroup )
				{
					qActionGroup = new QActionGroup( Q_APP() );
					qActionGroup->setObjectName( p_action.group.value() );
				}

				qAction->setCheckable( true );
				qActionGroup->addAction( qAction );
			}
			// Tip.
			if ( p_action.tip.has_value() )
			{
				QString tip = QString::fromStdString( p_action.tip.value() );

				if ( p_action.shortcut.has_value() )
				{
					tip.append( " (" + p_action.shortcut.value() + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( p_action.icon.has_value() )
			{
				if ( std::holds_alternative<int>( p_action.icon.value() ) )
				{
					qAction->setIcon(
						QApplication::style()->standardIcon(
							static_cast<QStyle::StandardPixmap>( std::get<int>( p_action.icon.value() ) )
						)
					);
				}
				else if ( std::holds_alternative<std::string>( p_action.icon.value() ) )
				{
					qAction->setIcon(
						QIcon( QString::fromStdString( ( ":/" + std::get<std::string>( p_action.icon.value() ) ) ) )
					);
				}
				else
				{
					VTX_ERROR( "Invalid icon type for action: {}", key );
				}
			}
			// Shortcut.
			if ( p_action.shortcut.has_value() )
			{
				qAction->setShortcut( QKeySequence( QString::fromStdString( p_action.shortcut.value() ) ) );
			}
			// Checkable.
			if ( p_action.checkable.has_value() )
			{
				qAction->setCheckable( p_action.checkable.value() );
			}
			// Action.
			if ( p_action.trigger.has_value() )
			{
				QObject::connect( qAction, &QAction::triggered, p_action.trigger.value() );
			}
			// Connect.
			// TODO: maybe this is dirty (calling this function to get previously created qAction).
			p_action.connect();
		}

		return qAction;
	}

} // namespace VTX::UI::QT
