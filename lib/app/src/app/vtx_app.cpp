#include "app/vtx_app.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/ecs/building/entity_director.hpp"
#include "app/ecs/building/setup_entity_director.hpp"
#include "app/ecs/entity/application/scene_entity.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <exception>
#include <util/logger.hpp>

namespace VTX::App
{
	// int ZERO = 0;
	VTXApp::VTXApp( StructPrivacyToken ) // : QApplication( ZERO, nullptr )
	{
		// connect( this, &QCoreApplication::aboutToQuit, this, &VTXApp::_stop );
	}

	VTXApp::~VTXApp() {}

	void VTXApp::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: {}", Old::Internal::IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

		//// Load settings.
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

		ECS::Building::setupEntityDirector();

		// Create scene.
		ECS::Core::BaseEntity sceneEntity
			= ECS::Building::EntityDirector::launchBuilder( ECS::Building::SCENE_ENTITY_ID );

		_scene = &( ECS::MAIN_REGISTRY().getComponent<Application::Scene>( sceneEntity ) );

		//_tickTimer.start();

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
	void VTXApp::update() { _update(); }
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

	void VTXApp::_update()
	{
		// Useless: nothing is delayed.
		// Event manager.
		// Event::EventManager::get().update( elapsed );

		// Action manager.
		// Action::ActionManager::get().update( elapsed );

		// Call late update event for processes at end of frame
		VTX_EVENT( Old::Event::Global::LATE_UPDATE );

		// TODO Reimplement this without Qt
		//// Tickrate.
		//_tickCounter++;
		// if ( _tickTimer.elapsed() >= 1000 )
		//{
		//	VTX_STAT().tickRate = _tickCounter / ( _tickTimer.elapsed() * 1e-3 );
		//	// VTX_INFO( "FPS: " + std::to_string( VTX_STAT().FPS ) + " - "
		//	//		  + "Tickrate: " + std::to_string( VTX_STAT().tickRate ) + " - "
		//	//		  + "Render time: " + std::to_string( VTX_STAT().renderTime ) + " ms" );
		//	_tickCounter = 0;
		//	_tickTimer.restart();
		//}
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
		// Prevent events throw for nothing when quitting app
		Old::Manager::EventManager::get().freezeEvent( true );
		// Manager::WorkerManager::get().stopAll();

		//_setting.backup();

		// VTX::MVC_MANAGER().deleteModel( _representationLibrary );
		// VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

		Old::Application::Selection::SelectionManager::get().deleteModel();

		// if ( _scene != nullptr )
		//{
		//	delete _scene;
		// }
	}

	void VTXApp::goToState( const std::string & p_name, void * const p_arg ) {}

	void VTXApp::renderScene() const {}

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
} // namespace VTX::App
