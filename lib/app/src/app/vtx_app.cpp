#include "app/vtx_app.hpp"
#include "app/application/action/action_manager.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/application/settings.hpp"
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
#include <renderer/renderer.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::App
{
	VTXApp::VTXApp( StructPrivacyToken ) {}

	VTXApp::~VTXApp() = default;

	void VTXApp::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: {}", IO::Internal::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

		// Load settings.
		// VTX_ACTION<Action::Setting::Load>();
		//_setting.loadRecentPaths();

		//// Create singletons.
		// MVC_MANAGER();
		// Manager::ActionManager::get();
		// Manager::EventManager::get();
		// Application::Selection::SelectionManager::get();
		// Manager::WorkerManager::get();

		//// Create Databases
		//_representationLibrary
		//	= MVC_MANAGER().instantiateModel<Application::Representation::RepresentationLibrary>();
		//_renderEffectLibrary = MVC_MANAGER().instantiateModel<Application::RenderEffect::RenderEffectLibrary>();
		//_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

		_system = std::make_shared<Application::System>();

		_registryManager = std::make_unique<Application::ECS::RegistryManager>();
		_system->referenceSystem( REGISTRY_MANAGER_KEY, _registryManager.get() );

		_workerManager = std::make_unique<Core::Worker::WorkerManager>();
		_system->referenceSystem( WORKER_MANAGER_KEY, _workerManager.get() );

		_actionManager = std::make_unique<Application::Action::ActionManager>();
		_system->referenceSystem( ACTION_MANAGER_KEY, _actionManager.get() );

		_serializationToolManager = std::make_unique<Core::Serialization::Serialization>();
		_system->referenceSystem( SERIALIZATION_TOOL_KEY, _serializationToolManager.get() );

		_entityDirector = std::make_unique<Application::ECS::EntityDirector>();
		_system->referenceSystem( ENTITY_DIRECTOR_KEY, _entityDirector.get() );
		Internal::ECS::setupEntityDirector();

		_selectionManager = std::make_unique<Application::Selection::SelectionManager>();
		_system->referenceSystem( SELECTION_MANAGER_KEY, _selectionManager.get() );

		_settings = std::make_unique<Application::Settings>();
		Internal::Application::Settings::initSettings( *_settings );

		_system->referenceSystem( SETTINGS_KEY, _settings.get() );

		// Create scene.
		Core::ECS::BaseEntity sceneEntity = getEntityDirector().build( Entity::SCENE_ENTITY_ID );
		Application::Scene &  scene		  = MAIN_REGISTRY().getComponent<Application::Scene>( sceneEntity );

		// TODO better way to manage this
		_system->referenceSystem( SCENE_KEY, &scene );

		// Create renderer
		_renderer
			= std::make_unique<Renderer::Renderer>( 1920, 1080, Util::Filesystem::getExecutableDir() / "shaders" );

		// Regsiter loop events
		_updateCallback.addCallback( this, []( const float p_elapsedTime ) { SCENE().update( p_elapsedTime ); } );

		_postUpdateCallback.addCallback( this, []( const float p_elapsedTime ) { THREADING().lateUpdate(); } );

		// Event manager - Useless: nothing is delayed.
		//_updateCallback.addCallback(
		//	this, []( const float p_elapsedTime ) { Event::EventManager::get().update( p_elapsedTime ); }
		//);

		// Useless while delayed actions are disabled
		//_updateCallback.addCallback( this, []( const float p_elapsedTime ) { VTX_ACTION().update( p_elapsedTime ); }
		//);

		// TODO: use camera callbacks.
		_preRenderCallback.addCallback( this, [ this ]( const float p_elapsedTime ) { _applyCameraUniforms(); } );
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
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _preUpdateCallback.call( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _updateCallback.call( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::LATE_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _lateUpdateCallback.call( p_elapsedTime ); } )
		);
		frameInfo.set(
			Internal::Monitoring::POST_UPDATE_DURATION_KEY,
			Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _postUpdateCallback.call( p_elapsedTime ); } )
		);

		if ( _renderer->getRenderGraph().isBuilt() )
		{
			frameInfo.set(
				Internal::Monitoring::PRE_RENDER_DURATION_KEY,
				Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _preRenderCallback.call( p_elapsedTime ); } )
			);
			frameInfo.set(
				Internal::Monitoring::RENDER_DURATION_KEY,
				Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _renderCallback.call( p_elapsedTime ); } )
			);
			frameInfo.set(
				Internal::Monitoring::POST_RENDER_DURATION_KEY,
				Util::CHRONO_CPU( [ this, p_elapsedTime ]() { _postRenderCallback.call( p_elapsedTime ); } )
			);
		}
	}
	void VTXApp::stop() { _stop(); }

	void VTXApp::_handleArgs( const std::vector<std::string> & p_args )
	{
		std::vector<FilePath>	 files	= std::vector<FilePath>();
		std::vector<std::string> pdbIds = std::vector<std::string>();

		for ( const std::string & arg : p_args )
		{
			if ( arg.find( "." ) != std::string::npos )
			{
				files.emplace_back( FilePath( arg ) );
			}
			else
			{
				pdbIds.emplace_back( arg );
			}
		}

		// if ( files.size() > 0 )
		//{
		//	VTX_ACTION<Action::Main::Open>( files );
		// }

		// for ( const std::string & pdbId : pdbIds )
		//{
		//	VTX_ACTION<Action::Main::OpenApi>( pdbId );
		// }
	}

	void VTXApp::_applyCameraUniforms() const
	{
		_renderer->setUniform( SCENE().getCamera().getViewMatrix(), "Matrix view" );
		_renderer->setUniform( SCENE().getCamera().getProjectionMatrix(), "Matrix projection" );
		_renderer->setCameraClipInfos( SCENE().getCamera().getNear(), SCENE().getCamera().getFar() );
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

	Application::Scene &	   VTXApp::getScene() { return _system->getSystem<Application::Scene>( SCENE_KEY ); }
	const Application::Scene & VTXApp::getScene() const { return _system->getSystem<Application::Scene>( SCENE_KEY ); }

	Application::Settings & VTXApp::getSettings() { return _system->getSystem<Application::Settings>( SETTINGS_KEY ); }
	const Application::Settings & VTXApp::getSettings() const
	{
		return _system->getSystem<Application::Settings>( SETTINGS_KEY );
	}

	Application::ECS::RegistryManager & VTXApp::getRegistryManager()
	{
		return _system->getSystem<Application::ECS::RegistryManager>( REGISTRY_MANAGER_KEY );
	}
	const Application::ECS::RegistryManager & VTXApp::getRegistryManager() const
	{
		return _system->getSystem<Application::ECS::RegistryManager>( REGISTRY_MANAGER_KEY );
	}

	Application::ECS::EntityDirector & VTXApp::getEntityDirector()
	{
		return _system->getSystem<Application::ECS::EntityDirector>( ENTITY_DIRECTOR_KEY );
	}

	Application::Selection::SelectionManager & VTXApp::getSelectionManager()
	{
		return _system->getSystem<Application::Selection::SelectionManager>( SELECTION_MANAGER_KEY );
	}
	const Application::Selection::SelectionManager & VTXApp::getSelectionManager() const
	{
		return _system->getSystem<Application::Selection::SelectionManager>( SELECTION_MANAGER_KEY );
	}

	Core::Serialization::Serialization & VTXApp::getSerializationTool()
	{
		return _system->getSystem<Core::Serialization::Serialization>( SERIALIZATION_TOOL_KEY );
	}
	const Core::Serialization::Serialization & VTXApp::getSerializationTool() const
	{
		return _system->getSystem<Core::Serialization::Serialization>( SERIALIZATION_TOOL_KEY );
	}

	Application::Action::ActionManager & VTXApp::getActionManager()
	{
		return _system->getSystem<Application::Action::ActionManager>( ACTION_MANAGER_KEY );
	}
	const Application::Action::ActionManager & VTXApp::getActionManager() const
	{
		return _system->getSystem<Application::Action::ActionManager>( ACTION_MANAGER_KEY );
	}

	Core::Worker::WorkerManager & VTXApp::getWorkerManager()
	{
		return _system->getSystem<Core::Worker::WorkerManager>( WORKER_MANAGER_KEY );
	}
	const Core::Worker::WorkerManager & VTXApp::getWorkerManager() const
	{
		return _system->getSystem<Core::Worker::WorkerManager>( WORKER_MANAGER_KEY );
	}

	Application::Scene &				SCENE() { return VTXApp::get().getScene(); }
	Renderer::Renderer &				RENDERER() { return VTXApp::get().getRenderer(); }
	Application::Settings &				SETTINGS() { return VTXApp::get().getSettings(); }
	Application::ECS::RegistryManager & MAIN_REGISTRY() { return VTXApp::get().getRegistryManager(); }
	Application::Selection::Selection & CURRENT_SELECTION() { return VTXApp::get().getSelectionManager().getCurrent(); }
	Core::Serialization::Serialization & SERIALIZER() { return VTXApp::get().getSerializationTool(); }
	Application::Action::ActionManager & VTX_ACTION() { return VTXApp::get().getActionManager(); }
	Core::Worker::WorkerManager &		 THREADING() { return VTXApp::get().getWorkerManager(); }

} // namespace VTX::App
