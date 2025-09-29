#include "app/vtx_app.hpp"
#include "app/action/application.hpp"
#include "app/action/mode.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/render/proxy_system.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/core/library/library_system.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/threading/base_thread.hpp"
#include "app/core/threading/threading_system.hpp"
#include "app/entity/scene.hpp"
#include "app/event_hub.hpp"
#include "app/events.hpp"
#include "app/filesystem.hpp"
#include "app/library/preset/color_layout.hpp"
#include "app/library/preset/render_settings.hpp"
#include "app/library/preset/representation.hpp"
#include "app/mode/visualization.hpp"
#include "app/monitoring/constants.hpp"
#include "app/python_binding/python_binding.hpp"
#include "app/python_binding/run_script.hpp"
#include "app/selection/selection_manager.hpp"
#include "app/settings.hpp"
#include "app/updater.hpp"
#include <core/struct/representation.hpp>
#include <exception>
#include <io/internal/filesystem.hpp>
#include <python_binding/interpretor.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/monitoring/stats.hpp>
//
#include "app/python_binding/interpretor.hpp"

namespace VTX::App
{

	VTXApp::VTXApp( const Args & p_args )
	{
		ECS::setRegistry( _registry );
		ECS::setCtx<Args>( p_args );
		ECS::setCtx<EventHub>();
	}

	void VTXApp::init()
	{
		VTX_DEBUG( "Init application" );

		// Load preset libraries.
		auto * lib	  = LIBRARY_SYSTEM().load<Library::Preset::Representation>( Filesystem::getRepresentationsDir() );
		auto * preset = lib->createPreset( "Sticks" );
		preset->setData( App::Library::Preset::Representations::STICKS );
		preset = lib->createPreset( "Balls and sticks" );
		preset->setData( App::Library::Preset::Representations::BALLS_AND_STICKS );
		preset = lib->createPreset( "Van der Waals" );
		preset->setData( App::Library::Preset::Representations::VAN_DER_WAALS );
		preset = lib->createPreset( "Ribbons" );
		preset->setData( App::Library::Preset::Representations::RIBBONS );
		preset = lib->createPreset( "SES" );
		preset->setData( App::Library::Preset::Representations::SES );

		LIBRARY_SYSTEM().load<Library::Preset::ColorLayout>( Filesystem::getColorLayoutsDir() );
		LIBRARY_SYSTEM().load<Library::Preset::RenderSettings>( Filesystem::getEffectsDir() );

		// TODO: move to start to handle gui dialog?
		Settings::initSettings();

		// Create scene.
		auto sceneEntity = ECS_REGISTRY().createEntity<Entity::Scene>();
		_scene			 = &ECS_REGISTRY().getComponent<Application::Scene>( sceneEntity );

		// Init tools.
		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->init();
		}

		// Register loop events.
		// onPostUpdate += []( const float p_elapsedTime ) { THREADING_SYSTEM().lateUpdate(); };

		// Initialize python interpretor.
		INTERPRETOR().subscribe(
			[]( VTX::PythonBinding::Interpretor & p_interpretor )
			{
				p_interpretor.add( VTX::App::PythonBinding::VTXAppBinder() );
				p_interpretor.add( VTX::App::PythonBinding::RunScript() );
			}
		);
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: {}", ECS::getCtx<Args>().toString() );

		// Build the renderer (graphic api backend context ready).
		auto & renderer = RENDERER_SYSTEM();

		if ( ECS::getCtx<Args>().has( ARG_NO_GRAPHICS ) )
		{
			VTX_WARNING( "No graphics" );
			renderer.setDefault();
		}
		else
		{
			try
			{
				renderer.setOpenGL45( Filesystem::getShadersDir() );
			}
			catch ( const std::exception & e )
			{
				VTX_ERROR( "Failed to build renderer: {}", e.what() );
				renderer.setDefault();
				// TODO: exit?
			}
		}

		// Connect render event.
		HUB().connect<Events::Render>( []( const Events::Render & p_e )
									   { RENDERER_SYSTEM().render( p_e.delta, p_e.elapsed ); } );

		// ?
		// Internal::initSettings( App::SETTINGS() );

		ACTION_SYSTEM().execute<Action::Mode::SetMode<Mode::Visualization>>();
		HUB().trigger<Events::ApplicationStarted>();

		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->onAppStart();
		}

		// Updater.
		UPDATER().onUpdateAvailable += []( const uint, const uint, const uint ) { UPDATER().downloadUpdate(); };

		if ( not ECS::getCtx<Args>().has( ARG_NO_UPDATE ) )
		{
			// UPDATER().checkForUpdate();
		}

		//_handleArgs( _args );
	}

	/*
	void VTXApp::update( const float p_deltaTime, const float p_elapsedTime )
	{
		Util::Monitoring::FrameInfo & frameInfo = STATS().newFrame();
		frameInfo.set(
			Monitoring::TICK_RATE_KEY,
			Util::CHRONO_CPU( [ p_deltaTime, p_elapsedTime ]() { _update( p_deltaTime, p_elapsedTime ); } )
		);
	}

	void VTXApp::_update( const float p_deltaTime, const float p_elapsedTime )
	{
		Util::Monitoring::FrameInfo & frameInfo = STATS().getCurrentFrame();


		frameInfo.set(
			Monitoring::PRE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreUpdate( p_elapsedTime ); } )
		);


	frameInfo.set(
		Monitoring::UPDATE_DURATION_KEY,
		Util::CHRONO_CPU( [ p_deltaTime, p_elapsedTime ]() { onUpdate( p_deltaTime, p_elapsedTime ); } )
	);


	frameInfo.set(
		Monitoring::LATE_UPDATE_DURATION_KEY,
		Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onLateUpdate( p_elapsedTime ); } )
	);


	frameInfo.set(
		Monitoring::POST_UPDATE_DURATION_KEY,
		Util::CHRONO_CPU( [ p_elapsedTime ]() { onPostUpdate( p_elapsedTime ); } )
	);


	frameInfo.set(
		Monitoring::PRE_RENDER_DURATION_KEY,
		Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreRender( p_elapsedTime ); } )
	);


	frameInfo.set(
		Monitoring::RENDER_DURATION_KEY,
		Util::CHRONO_CPU( [ p_deltaTime, p_elapsedTime ]() { RENDERER_SYSTEM().render( p_deltaTime, p_elapsedTime ); } )
	);

	frameInfo.set(
		Monitoring::POST_RENDER_DURATION_KEY,
		Util::CHRONO_CPU( [ p_elapsedTime ]() { onPostRender( p_elapsedTime ); } )
	);

	frameInfo.set(
		Monitoring::END_OF_FRAME_ONE_SHOT_DURATION_KEY,
		Util::CHRONO_CPU(
			[ p_elapsedTime ]()
			{
				onEndOfFrameOneShot();
				onEndOfFrameOneShot.clear();
			}
		)
	);
}
*/

	void VTXApp::stop()
	{
		VTX_INFO( "Stopping application" );

		SCENE().reset();
		RENDERER_SYSTEM().clean();

		//// Prevent events throw for nothing when quitting app
		// Old::Manager::EventManager::get().freezeEvent( true );
		//  Manager::WorkerManager::get().stopAll();

		//_setting.backup();

		// VTX::MVC_MANAGER().deleteModel( _representationLibrary );
		// VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

		// Old::Selection::SelectionManager::get().deleteModel();

		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->onAppStop();
		}
		HUB().trigger<Events::ApplicationStopped>();
	}

	void VTXApp::_handleArgs( const Args & args )
	{
		// TODO: load pdb automatically or python script.

		/*
		using FILE_TYPE_ENUM = IO::Internal::Filesystem::FILE_TYPE_ENUM;
		for ( const auto arg : args.all() )
		{
			// If argument is an existing file
			if ( std::filesystem::exists( arg ) )
			{
				const FilePath		 path	  = FilePath( arg );
				const FILE_TYPE_ENUM fileType = IO::Internal::Filesystem::getFileTypeFromFilePath( path );

				try
				{
					switch ( fileType )
					{
					case FILE_TYPE_ENUM::MOLECULE:
					case FILE_TYPE_ENUM::TRAJECTORY:
						App::ACTION_SYSTEM().execute<App::Action::Scene::LoadSystem>( arg );
						break;

					case FILE_TYPE_ENUM::SCENE:
						App::ACTION_SYSTEM().execute<App::Action::Application::OpenScene>( arg );
						break;

					case FILE_TYPE_ENUM::SCRIPT:
						// App::VTX_ACTION().execute<PythonBinding::Action::RunScript>( arg );
						break;
					}
				}
				catch ( const IOException & p_e )
				{
					VTX_ERROR( "Can't open file '{}' : {}.", arg, p_e.what() );
				}
			}
			// If argument is a system name.
			else if ( arg.size() == 4 )
			{
				// Check only letter and number.
				if ( std::all_of( arg.begin(), arg.end(), []( const char c ) { return std::isalnum( c ); } ) )
				{
					App::ACTION_SYSTEM().execute<App::Action::Scene::DownloadSystem>(
						arg, std::string( arg ) + ".pdb"
					);
				}
				else
				{
					VTX_WARNING( "Argument '{}' is not a valid system name.", arg );
				}
			}
			else
			{
				VTX_WARNING( "Argument '{}' is not valid.", arg );
			}
		}
		*/
	}

	//	bool VTXApp::hasAnyModifications() const
	//	{
	//		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
	//		const bool sceneIsEmpty	   = getScene().isEmpty();
	//		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
	// #ifdef VTX_PRODUCTION
	//		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
	// #else
	//		return hasSavePath && sceneHasChanged;
	// #endif
	//	}

	// TODO.
	Application::Scene &	  SCENE() { return APP::getScene(); }
	Util::Monitoring::Stats & STATS() { return Util::Singleton<Util::Monitoring::Stats>::get(); }
	Updater &				  UPDATER() { return Util::Singleton<Updater>::get(); }

} // namespace VTX::App
