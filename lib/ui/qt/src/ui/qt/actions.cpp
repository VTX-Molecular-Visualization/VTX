#include "ui/qt/actions.hpp"
#include "app/action/application.hpp"
#include "app/action/controller.hpp"
#include "app/action/scene.hpp"
#include "app/action/selection.hpp"
#include "app/setting/controller.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/dialog/download.hpp"
#include "ui/qt/dialog/export_image.hpp"
#include "ui/qt/dialog/open.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/color.hpp>
#include <app/action/io.hpp>
#include <app/action/representation.hpp>
#include <app/action/visibility.hpp>
#include <app/constants.hpp>
#include <app/system/selection.hpp>
#include <string>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <utility>

namespace VTX::UI::QT::Action
{
	void registerActions( ActionRegistry & p_registry )
	{
		const auto handler = []( auto p_callback )
		{ return [ callback = std::move( p_callback ) ]( const ActionRegistry::ActionContext & ) { callback(); }; };

		p_registry.registerAction(
			System::newAction(), handler( []() { App::ACTION().execute<App::Action::Scene::Clear>(); } )
		);
		p_registry.registerAction(
			System::downloadAction(),
			handler(
				[]()
				{
					Dialog::Download dialog;
					dialog.exec();
				}
			)
		);
		p_registry.registerAction(
			System::openAction(),
			handler(
				[]()
				{
					Dialog::Open dialog;
					dialog.exec();
				}
			)
		);
		p_registry.registerAction( System::openRecentAction() );
		p_registry.registerAction( System::saveAction() );
		p_registry.registerAction( System::saveAsAction() );
		p_registry.registerAction( System::importAction() );
		p_registry.registerAction( System::exportAction() );
		p_registry.registerAction( System::quitAction(), handler( []() { QCoreApplication::quit(); } ) );

		p_registry.registerAction(
			Camera::orthographicAction(),
			handler(
				[]()
				{ App::ACTION().execute<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>(); }
			)
		);
		p_registry.registerAction(
			Camera::perspectiveAction(),
			handler(
				[]()
				{ App::ACTION().execute<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>(); }
			)
		);
		p_registry.registerAction(
			Camera::trackballAction(),
			handler(
				[]()
				{
					App::ACTION()
						.execute<App::Action::Controller::SetCameraController<App::Setting::E_CONTROLLER::TRACKBALL>>();
				}
			)
		);
		p_registry.registerAction(
			Camera::freeflyAction(),
			handler(
				[]()
				{
					App::ACTION()
						.execute<App::Action::Controller::SetCameraController<App::Setting::E_CONTROLLER::FREEFLY>>();
				}
			)
		);
		p_registry.registerAction(
			Camera::orientAction(), handler( []() { App::ACTION().execute<App::Action::Camera::Orient>(); } )
		);
		p_registry.registerAction(
			Camera::resetAction(), handler( []() { App::ACTION().execute<App::Action::Camera::Reset>(); } )
		);
		p_registry.registerAction(
			Camera::saveViewpointAction(),
			handler( []() { App::ACTION().execute<App::Action::Camera::SaveViewpoint>(); } )
		);

		p_registry.registerAction(
			Snapshot::snapshotAction(),
			handler(
				[]()
				{
					App::Action::IO::Snapshot action;
					App::ACTION().execute( action );
				}
			)
		);
		p_registry.registerAction(
			Snapshot::exportAction(),
			handler(
				[]()
				{
					Dialog::ExportImage dialog;
					dialog.exec();
				}
			)
		);

		p_registry.registerAction(
			View::fullscreenAction(),
			handler(
				[]()
				{
					if ( MAIN_WINDOW().isFullScreen() )
					{
						MAIN_WINDOW().showNormal();
					}
					else
					{
						MAIN_WINDOW().showFullScreen();
					}
				}
			)
		);

		p_registry.registerAction(
			Selection::lockAction(),
			[]( const ActionRegistry::ActionContext & p_context )
			{
				const bool locked = UI_ACTIONS().isChecked( p_context.actionId );
				App::HUB().trigger<Events::SelectionLocked>( locked );
			}
		);
		p_registry.registerAction( Selection::saveAction() );
		p_registry.registerAction(
			Selection::clearAction(),
			[]( const ActionRegistry::ActionContext & )
			{
				if ( UI_ACTIONS().isChecked( Selection::LOCK ) )
				{
					return;
				}

				SELECTION().clearSystem();
			}
		);
		p_registry.registerAction(
			Selection::selectAllAction(),
			handler( []() { App::ACTION().execute<App::Action::Selection::SelectAll>(); } )
		);
		p_registry.registerAction(
			Selection::setGranularitySystemAction(),
			handler(
				[]()
				{
					const int granularity = toUnderlying( App::Action::Selection::E_GRANULARITY::SYSTEM );
					SETTINGS().setValue( SETTING_KEY_GRANULARITY, granularity );
					App::HUB().trigger<Events::SelectionGranularityChanged>( granularity );
				}
			)
		);
		p_registry.registerAction(
			Selection::setGranularityChainAction(),
			handler(
				[]()
				{
					const int granularity = toUnderlying( App::Action::Selection::E_GRANULARITY::CHAIN );
					SETTINGS().setValue( SETTING_KEY_GRANULARITY, granularity );
					App::HUB().trigger<Events::SelectionGranularityChanged>( granularity );
				}
			)
		);
		p_registry.registerAction(
			Selection::setGranularityResidueAction(),
			handler(
				[]()
				{
					const int granularity = toUnderlying( App::Action::Selection::E_GRANULARITY::RESIDUE );
					SETTINGS().setValue( SETTING_KEY_GRANULARITY, granularity );
					App::HUB().trigger<Events::SelectionGranularityChanged>( granularity );
				}
			)
		);
		p_registry.registerAction(
			Selection::setGranularityAtomAction(),
			handler(
				[]()
				{
					const int granularity = toUnderlying( App::Action::Selection::E_GRANULARITY::ATOM );
					SETTINGS().setValue( SETTING_KEY_GRANULARITY, granularity );
					App::HUB().trigger<Events::SelectionGranularityChanged>( granularity );
				}
			)
		);
		p_registry.registerAction(
			Selection::showAction(),
			handler( []() { App::ACTION().execute<App::Action::Visibility::SetVisibleSelected>( true ); } )
		);
		p_registry.registerAction(
			Selection::hideAction(),
			handler( []() { App::ACTION().execute<App::Action::Visibility::SetVisibleSelected>( false ); } )
		);
		p_registry.registerAction( Selection::soloAction() );
		p_registry.registerAction(
			Selection::setColorSchemeAction(),
			[]( const ActionRegistry::ActionContext & p_context )
			{
				const auto schemeParam	   = p_context.param<int>( Selection::PARAM_COLOR_SCHEME );
				const auto colorIndexParam = p_context.param<int>( Selection::PARAM_COLOR_INDEX );
				if ( schemeParam )
				{
					App::ACTION().execute<App::Action::Color::AddSelected>(
						static_cast<Renderer::E_COLOR_SCHEME>( *schemeParam ), colorIndexParam
					);
				}
			}
		);
		p_registry.registerAction(
			Selection::setRepresentationAction(),
			[]( const ActionRegistry::ActionContext & p_context )
			{
				const auto representationParam = p_context.param<int>( Selection::PARAM_REPRESENTATION );
				if ( representationParam )
				{
					App::ACTION().execute<App::Action::Representation::AddSelected>(
						static_cast<Entity>( *representationParam )
					);
				}
			}
		);
		p_registry.registerAction(
			Selection::deleteAction(),
			handler( []() { App::ACTION().execute<App::Action::Scene::DeleteSystemSelected>(); } )
		);

		p_registry.registerAction(
			Theme::setAction(),
			[]( const ActionRegistry::ActionContext & p_context )
			{
				const auto theme = p_context.param<int>( Theme::PARAM_THEME );
				if ( theme )
				{
					STYLE().setTheme( static_cast<Style::E_THEME>( *theme ) );
					App::HUB().trigger<Events::ThemeChanged>( *theme );
				}
			}
		);
		p_registry.registerAction( Theme::resetLayoutAction(), handler( []() { MAIN_WINDOW().resetLayout(); } ) );
		p_registry.registerAction( Theme::setFontAction() );

		p_registry.registerAction( Option::Cache::openAction() );
		p_registry.registerAction( Option::Cache::clearAction() );
		p_registry.registerAction( Option::Cache::refreshAction() );

		p_registry.registerAction(
			Help::documentationAction(),
			handler( []() { QDesktopServices::openUrl( QUrl( App::URL_DOCUMENTATION.data() ) ); } )
		);
		p_registry.registerAction(
			Help::reportAction(), handler( []() { QDesktopServices::openUrl( QUrl( App::URL_REPORT.data() ) ); } )
		);
		p_registry.registerAction(
			Help::checkUpdatesAction(),
			handler( []() { App::ACTION().execute<App::Action::Application::CheckForUpdate>(); } )
		);
		p_registry.registerAction( Help::aboutAction() );
	}

} // namespace VTX::UI::QT::Action
