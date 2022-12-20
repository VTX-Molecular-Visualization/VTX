#include "vtx_app_v2.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/struct/scene_path_data.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/program_manager.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "worker/worker_manager.hpp"
#include <exception>

namespace VTX::App
{
	VTXAppV2::VTXAppV2() {}

	VTXAppV2::~VTXAppV2() {}

	void VTXAppV2::start( const std::vector<std::string> & p_args )
	{
		// Load settings.
		VTX_ACTION( new Action::Setting::Load() );
		//_setting.loadRecentPaths();

		// Create singletons.
		MVC::MvcManager::get();
		Action::ActionManager::get();
		Event::EventManager::get();
		Selection::SelectionManager::get();
		Worker::WorkerManager::get();

		// Create Databases
		//_representationLibrary
		//	= MVC::MvcManager::get().instantiateModel<Model::Representation::RepresentationLibrary>();
		//_renderEffectLibrary = MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPresetLibrary>();
		//_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

		// Create scene.
		//_scene = new Object3D::Scene();
		//_scene->getCamera().setScreenSize( Style::WINDOW_WIDTH_DEFAULT, Style::WINDOW_HEIGHT_DEFAULT );

		//_pathSceneData = new IO::Struct::ScenePathData();

		VTX_INFO( "Application started" );

		// if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
		//{
		//	VTX_ACTION( new Action::Main::CheckForUpdate() );
		// }
	}

	void VTXAppV2::update()
	{
		// Elapsed time.
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		// State machine.
		//_stateMachine->update( elapsed );

		// Useless: nothing is delayed.
		// Event manager.
		// Event::EventManager::get().update( elapsed );

		// Action manager.
		// Action::ActionManager::get().update( elapsed );

		_applyEndOfFrameDeletes();

		// Call late update event for processes at end of frame
		VTX_EVENT( new Event::VTXEvent( Event::Global::LATE_UPDATE ) );

		// Tickrate.
		_tickCounter++;
		// if ( _tickTimer.elapsed() >= 1000 )
		//{
		//	VTX_STAT().tickRate = _tickCounter / ( _tickTimer.elapsed() * 1e-3 );
		//	// VTX_INFO( "FPS: " + std::to_string( VTX_STAT().FPS ) + " - "
		//	//		  + "Tickrate: " + std::to_string( VTX_STAT().tickRate ) + " - "
		//	//		  + "Render time: " + std::to_string( VTX_STAT().renderTime ) + " ms" );
		//	_tickCounter = 0;
		//	_tickTimer.restart();
		// }
	}

	bool VTXAppV2::hasAnyModifications() const
	{
		return false;
		//		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
		//		const bool sceneIsEmpty	   = getScene().isEmpty();
		//		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
		// #ifdef VTX_PRODUCTION
		//		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
		// #else
		//		return hasSavePath && sceneHasChanged;
		// #endif
	}

	void VTXAppV2::stop()
	{
		// _timer can be uninitialized if critical error append during start (i.e. OpenGL init fail)
		if ( _timer != nullptr )
		{
			_timer->stop();
			delete _timer;
		}
		// Prevent events throw for nothing when quitting app
		Event::EventManager::get().freezeEvent( true );
		Worker::WorkerManager::get().stopAll();

		//_setting.backup();

		// MVC::MvcManager::get().deleteModel( _representationLibrary );
		// MVC::MvcManager::get().deleteModel( _renderEffectLibrary );

		Selection::SelectionManager::get().deleteModel();

		// if ( _scene != nullptr )
		//{
		//	delete _scene;
		// }
	}

	void VTXAppV2::goToState( const std::string & p_name, void * const p_arg )
	{
		// try
		//{
		//	_stateMachine->goToState( p_name, p_arg );
		// }
		// catch ( const std::exception & p_e )
		//{
		//	VTX_ERROR( p_e.what() );
		// }
	}

	void VTXAppV2::renderScene() const
	{
		// if ( VTX_SETTING().getActivateRenderer() && MASK )
		//{
		//	_mainWindow->updateRender();
		// }
	}

	void VTXAppV2::deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object )
	{
		_deleteAtEndOfFrameObjects.emplace_back( p_object );
	}
	void VTXAppV2::_applyEndOfFrameDeletes()
	{
		for ( const Generic::BaseAutoDelete * const p_object : _deleteAtEndOfFrameObjects )
			p_object->autoDelete();

		_deleteAtEndOfFrameObjects.clear();
	}

	// Model::Renderer::RenderEffectPreset & VTX_RENDER_EFFECT()
	//{
	//	return Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPreset();
	// }

} // namespace VTX::App
