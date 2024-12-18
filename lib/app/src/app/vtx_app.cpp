#include "app/vtx_app.hpp"
#include "app/action/animation.hpp"
#include "app/action/application.hpp"
#include "app/action/mode.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/render/proxy_system.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/threading/base_thread.hpp"
#include "app/core/threading/threading_system.hpp"
#include "app/entity/scene.hpp"
#include "app/filesystem.hpp"
#include "app/mode/visualization.hpp"
#include "app/monitoring/constants.hpp"
#include "app/settings.hpp"
#include <exception>
#include <io/internal/filesystem.hpp>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App
{

	void VTXApp::init()
	{
		VTX_DEBUG( "Init application" );

		Settings::initSettings();

		// Init renderer.
		Core::Renderer::RendererSystem::init( Filesystem::getShadersDir() );

		// Create scene.
		auto sceneEntity = ECS_REGISTRY().createEntity<Entity::Scene>();
		_scene			 = &ECS_REGISTRY().getComponent<Application::Scene>( sceneEntity );

		// Init tools.
		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->init();
		}

		// Register loop events.
		onPostUpdate += []( const float p_elapsedTime ) { THREADING_SYSTEM().lateUpdate(); };

		// Create Databases
		//_representationLibrary
		//	= MVC_MANAGER().instantiateModel<Application::Representation::RepresentationLibrary>();
		//_renderEffectLibrary = MVC_MANAGER().instantiateModel<Application::RenderEffect::RenderEffectLibrary>();
		//_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );
	}

	void VTXApp::start( const Args & args )
	{
		VTX_INFO( "Starting application: {}", args.toString() );

		// TODO: use custom context? (for offline rendering i CLI mode?)
		// Builid the renderer (graphic api backend context ready).
		auto & renderer = RENDERER_SYSTEM();
		renderer.build();

		// ?
		// Internal::initSettings( App::SETTINGS() );

		ACTION_SYSTEM().execute<Action::Mode::SetMode<Mode::Visualization>>();

		onStart();
		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->onAppStart();
		}

		_handleArgs( args );
	}

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

		/*
		frameInfo.set(
			Monitoring::PRE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreUpdate( p_elapsedTime ); } )
		);
		*/

		frameInfo.set(
			Monitoring::UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ p_deltaTime, p_elapsedTime ]() { onUpdate( p_deltaTime, p_elapsedTime ); } )
		);

		/*
		frameInfo.set(
			Monitoring::LATE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onLateUpdate( p_elapsedTime ); } )
		);
		*/

		frameInfo.set(
			Monitoring::POST_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ p_elapsedTime ]() { onPostUpdate( p_elapsedTime ); } )
		);

		/*
		frameInfo.set(
			Monitoring::PRE_RENDER_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreRender( p_elapsedTime ); } )
		);
		*/

		frameInfo.set(
			Monitoring::RENDER_DURATION_KEY,
			Util::CHRONO_CPU( [ p_deltaTime, p_elapsedTime ]()
							  { RENDERER_SYSTEM().render( p_deltaTime, p_elapsedTime ); } )
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

	void VTXApp::stop()
	{
		VTX_INFO( "Stopping application" );

		SCENE().reset();

		//// Prevent events throw for nothing when quitting app
		// Old::Manager::EventManager::get().freezeEvent( true );
		//  Manager::WorkerManager::get().stopAll();

		//_setting.backup();

		// VTX::MVC_MANAGER().deleteModel( _representationLibrary );
		// VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

		// Old::Application::Selection::SelectionManager::get().deleteModel();

		for ( Tool::BaseTool * const tool : _tools )
		{
			tool->onAppStop();
		}
		onStop();
	}

	void VTXApp::_handleArgs( const Args & args )
	{
		using FILE_TYPE_ENUM = IO::Internal::Filesystem::FILE_TYPE_ENUM;
		for ( const std::string & arg : args.all() )
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
					App::ACTION_SYSTEM().execute<App::Action::Scene::DownloadSystem>( arg, arg + ".pdb" );
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

} // namespace VTX::App
