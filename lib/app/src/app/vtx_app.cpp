#include "app/vtx_app.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/application/settings.hpp"
#include "app/application/system/renderer.hpp"
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
#include <exception>
#include <io/internal/filesystem.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App
{
	VTXApp::VTXApp( StructPrivacyToken ) {}
	VTXApp::~VTXApp() = default;

	void VTXApp::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: {}", IO::Internal::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

		//// Create Databases
		//_representationLibrary
		//	= MVC_MANAGER().instantiateModel<Application::Representation::RepresentationLibrary>();
		//_renderEffectLibrary = MVC_MANAGER().instantiateModel<Application::RenderEffect::RenderEffectLibrary>();
		//_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

		_settings = std::make_unique<Application::Settings>();
		_systemHandlerPtr->reference( SETTINGS_KEY, _settings.get() );
		Internal::Application::Settings::initSettings( *_settings );

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

		// Event manager - Useless: nothing is delayed.
		//_updateCallback.addCallback(
		//	this, []( const float p_elapsedTime ) { Event::EventManager::get().update( p_elapsedTime ); }
		//);

		// Useless while delayed actions are disabled
		//_updateCallback.addCallback( this, []( const float p_elapsedTime ) { VTX_ACTION().update( p_elapsedTime );
		//}
		//);

		// TODO: use camera callbacks.
		//_preRenderCallback.addCallback( this, [ this ]( const float p_elapsedTime ) { _applyCameraUniforms(); } );
		//_renderCallback.addCallback(
		//	this, [ this ]( const float p_elapsedTime ) { _renderer->render( p_elapsedTime ); }
		//);

		_tickChrono.start();

		_handleArgs( p_args );

#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION(
			//	 new Action::Main::Open( Internal::IO::Filesystem::getDataPath( FilePath( "4hhb.pdb" )
			//).absolute() ) );
			// VTX_ACTION( new Action::Main::OpenApi( "1aon" ) );
			// VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );
			// VTX_ACTION( new Action::Main::OpenApi( "1aga" ) );
		}
#endif
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

	void VTXApp::_handleArgs( const std::vector<std::string> & p_args ) {}

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

		// if ( _scene != nullptr )
		//{
		//	delete _scene;
		// }
	}

	void VTXApp::goToState( const std::string & p_name, void * const p_arg ) {}

	// void VTXApp::deleteAtEndOfFrame( const Component::Generic::BaseAutoDelete * const p_object )
	//{
	//	_deleteAtEndOfFrameObjects.emplace_back( p_object );
	// }
	// void VTXApp::_applyEndOfFrameDeletes()
	//{
	//	for ( const Component::Generic::BaseAutoDelete * const p_object : _deleteAtEndOfFrameObjects )
	//		p_object->autoDelete();

	//	_deleteAtEndOfFrameObjects.clear();
	//}

	Application::Scene &	   VTXApp::getScene() { return _systemHandlerPtr->get<Application::Scene>( SCENE_KEY ); }
	const Application::Scene & VTXApp::getScene() const
	{
		return _systemHandlerPtr->get<Application::Scene>( SCENE_KEY );
	}

	Application::Settings &		  VTXApp::getSettings() { return *_settings; }
	const Application::Settings & VTXApp::getSettings() const { return *_settings; }

	Application::Scene &	SCENE() { return VTXApp::get().getScene(); }
	Application::Settings & SETTINGS() { return VTXApp::get().getSettings(); }

} // namespace VTX::App
