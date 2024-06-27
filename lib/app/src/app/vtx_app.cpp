#include "app/vtx_app.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/application/system/renderer.hpp"
#include "app/application/system/settings_system.hpp"
#include "app/application/system/threading.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/core/worker/worker_manager.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/application/scene_entity.hpp"
#include "app/internal/application/settings.hpp"
#include "app/internal/ecs/setup_entity_director.hpp"
#include "app/internal/monitoring/all_metrics.hpp"
#include "app/internal/serialization/all_serializers.hpp"
#include <exception>
#include <io/internal/filesystem.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
namespace VTX::App
{

	VTXApp::VTXApp() { VTX_DEBUG( "VTXApp::VTXApp()" ); }

	VTXApp::~VTXApp() = default;

	void VTXApp::start( const Args & p_args )
	{
		VTX_INFO( "Starting application: {}", IO::Internal::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

		//// Create Databases
		//_representationLibrary
		//	= MVC_MANAGER().instantiateModel<Application::Representation::RepresentationLibrary>();
		//_renderEffectLibrary = MVC_MANAGER().instantiateModel<Application::RenderEffect::RenderEffectLibrary>();
		//_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

		Internal::Application::Settings::initSettings( SETTINGS() );

		// Load settings.
		// VTX_ACTION<Action::Setting::Load>();
		//_setting.loadRecentPaths();

		Internal::ECS::setupEntityDirector();

		// Create scene.
		Core::ECS::BaseEntity sceneEntity = ENTITY_DIRECTOR().build( Entity::SCENE_ENTITY_ID );
		Application::Scene &  scene		  = MAIN_REGISTRY().getComponent<Application::Scene>( sceneEntity );

		// TODO better way to manage this
		_systemHandlerPtr->reference( SCENE_KEY, &scene );

		// Create renderer
		RENDERER().get().init();

		// Regsiter loop events
		onUpdate += []( const float p_elapsedTime ) { SCENE().update( p_elapsedTime ); };
		onPostUpdate += []( const float p_elapsedTime ) { THREADING().lateUpdate(); };

		_tickChrono.start();

		//_handleArgs( p_args );

		// Internal::initSettings( App::SETTINGS() );

		//_currentMode = std::make_unique<App::Mode::Visualization>();
		//_currentMode->enter();

		onStart();
	}

	void VTXApp::update( const float p_elapsedTime )
	{
		const float deltaTime = p_elapsedTime > 0 ? p_elapsedTime : _tickChrono.intervalTime();

		Core::Monitoring::FrameInfo & frameInfo = _stats.newFrame();
		frameInfo.set(
			Internal::Monitoring::TICK_RATE_KEY, Util::CHRONO_CPU( [ this, deltaTime ]() { _update( deltaTime ); } )
		);
	}

	void VTXApp::_update( const float p_elapsedTime )
	{
		Core::Monitoring::FrameInfo & frameInfo = _stats.getCurrentFrame();

		frameInfo.set(
			Internal::Monitoring::PRE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreUpdate( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onUpdate( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::LATE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onLateUpdate( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::POST_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPostUpdate( p_elapsedTime ); } )
		);

		frameInfo.set(
			Internal::Monitoring::PRE_RENDER_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPreRender( p_elapsedTime ); } )
		);

		frameInfo.set(
			Internal::Monitoring::RENDER_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onRender( p_elapsedTime ); } )
		);

		frameInfo.set(
			Internal::Monitoring::POST_RENDER_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { onPostRender( p_elapsedTime ); } )
		);

		frameInfo.set(
			Internal::Monitoring::END_OF_FRAME_ONE_SHOT_DURATION_KEY,
			Util::CHRONO_CPU(
				[ this, p_elapsedTime ]()
				{
					onEndOfFrameOneShot();
					onEndOfFrameOneShot.clear();
				}
			)
		);
	}
	void VTXApp::stop()
	{
		onStop();

		_stop();
	}

	void VTXApp::_handleArgs( const std::vector<std::string> & p_args )
	{
		/*

			using FILE_TYPE_ENUM = VTX::IO::Internal::Filesystem::FILE_TYPE_ENUM;
				for ( const std::string & p_arg : p_args )
				{
					// If argument is an existing file
					if ( std::filesystem::exists( p_arg ) )
					{
						const FilePath		 path	  = FilePath( p_arg );
						const FILE_TYPE_ENUM fileType = VTX::IO::Internal::Filesystem::getFileTypeFromFilePath( path );

						try
						{
							switch ( fileType )
							{
							case FILE_TYPE_ENUM::MOLECULE:
							case FILE_TYPE_ENUM::TRAJECTORY:
								App::VTX_ACTION().execute<App::Action::Scene::LoadMolecule>( p_arg );
								break;

							case FILE_TYPE_ENUM::SCENE:
								App::VTX_ACTION().execute<App::Action::Application::OpenScene>( p_arg );
								break;

							case FILE_TYPE_ENUM::SCRIPT:
								App::VTX_ACTION().execute<PythonBinding::Action::RunScript>( p_arg );
								break;

							default: throw IOException( "Unrecognized file" );
							}
						}
						catch ( const IOException & p_e )
						{
							VTX_ERROR( "Can't open file '{}' : {}.", p_arg, p_e.what() );
							break;
						}
					}
				}

		#ifndef VTX_PRODUCTION
				if ( p_args.size() == 0 )
				{
					// VTX_ACTION(
					//	 new App::Old::Action::Main::Open( Util::Filesystem::getDataPath( FilePath( "4hhb.pdb" )
		).absolute() )
					//);
					// App::Application::VTX_ACTION( new App::Old::Action::Main::OpenApi( "1aga" ) );
				}
		#endif

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

	void VTXApp::_stop()
	{
		_tickChrono.stop();

		//// Prevent events throw for nothing when quitting app
		// Old::Manager::EventManager::get().freezeEvent( true );
		//  Manager::WorkerManager::get().stopAll();

		//_setting.backup();

		// VTX::MVC_MANAGER().deleteModel( _representationLibrary );
		// VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

		// Old::Application::Selection::SelectionManager::get().deleteModel();
	}

	Application::Scene &	   VTXApp::getScene() { return _systemHandlerPtr->get<Application::Scene>( SCENE_KEY ); }
	const Application::Scene & VTXApp::getScene() const
	{
		return _systemHandlerPtr->get<Application::Scene>( SCENE_KEY );
	}

	Application::Scene & SCENE() { return APP().getScene(); }

	Mode::BaseMode & MODE() { return APP().getCurrentMode(); }

} // namespace VTX::App
