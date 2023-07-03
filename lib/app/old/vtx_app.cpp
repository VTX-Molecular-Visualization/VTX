#include "app/old/vtx_app.hpp"
#include "app/old/action/main.hpp"
#include "app/old/action/setting.hpp"
#include "app/old/application/render_effect/render_effect_library.hpp"
#include "app/old/application/render_effect/render_effect_preset.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/core/event/vtx_event.hpp"
#include "app/old/event.hpp"
#include "app/old/event/global.hpp"
#include "app/old/internal/io/serialization/scene_path_data.hpp"
#include "app/old/manager/action_manager.hpp"
#include "app/old/mvc.hpp"
#include "app/old/render/renderer/gl/program_manager.hpp"
// #include "ui/dialog.hpp"
// #include "ui/main_window.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/worker.hpp"
// #include <QApplication>
// #include <QPalette>
#include <exception>

namespace VTX
{
	namespace App::Old
	{
		// int ZERO = 0;
		VTXApp::VTXApp() // : QApplication( ZERO, nullptr )
		{
			// connect( this, &QCoreApplication::aboutToQuit, this, &VTXApp::_stop );
		}

		VTXApp::~VTXApp() {}

		void VTXApp::start( const std::vector<std::string> & p_args )
		{
			VTX_INFO( "Starting application: {}", Internal::IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );

			// Load settings.
			VTX_ACTION<Action::Setting::Load>();
			_setting.loadRecentPaths();

			// Create singletons.
			MVC_MANAGER();
			Manager::ActionManager::get();
			Manager::EventManager::get();
			Application::Selection::SelectionManager::get();
			Manager::WorkerManager::get();

			// Create Databases
			_representationLibrary
				= MVC_MANAGER().instantiateModel<Application::Representation::RepresentationLibrary>();
			_renderEffectLibrary = MVC_MANAGER().instantiateModel<Application::RenderEffect::RenderEffectLibrary>();
			_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

			// Create scene.
			_scene		   = new Application::Scene();
			_pathSceneData = new Internal::IO::Serialization::ScenePathData();

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

		void VTXApp::_initQt()
		{
			//		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );
			//
			//		QPalette appPalette = palette();
			//		Style::applyApplicationPaletteInPalette( appPalette );
			//		setPalette( appPalette );
			//
			// #ifdef _DEBUG
			//		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
			// #endif
		}

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

			if ( files.size() > 0 )
			{
				VTX_ACTION<Action::Main::Open>( files );
			}

			for ( const std::string & pdbId : pdbIds )
			{
				VTX_ACTION<Action::Main::OpenApi>( pdbId );
			}
		}

		void VTXApp::_update()
		{
			// Useless: nothing is delayed.
			// Event manager.
			// Event::EventManager::get().update( elapsed );

			// Action manager.
			// Action::ActionManager::get().update( elapsed );

			_applyEndOfFrameDeletes();

			// Call late update event for processes at end of frame
			VTX_EVENT( Event::Global::LATE_UPDATE );

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

		bool VTXApp::hasAnyModifications() const
		{
			const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
			const bool sceneIsEmpty	   = getScene().isEmpty();
			const bool sceneHasChanged = getScenePathData().sceneHasModifications();
#ifdef VTX_PRODUCTION
			return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
#else
			return hasSavePath && sceneHasChanged;
#endif
		}

		void VTXApp::_stop()
		{
			// Prevent events throw for nothing when quitting app
			Manager::EventManager::get().freezeEvent( true );
			Manager::WorkerManager::get().stopAll();

			_setting.backup();

			VTX::MVC_MANAGER().deleteModel( _representationLibrary );
			VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

			Application::Selection::SelectionManager::get().deleteModel();

			if ( _scene != nullptr )
			{
				delete _scene;
			}
		}

		void VTXApp::goToState( const std::string & p_name, void * const p_arg ) {}

		void VTXApp::renderScene() const {}

		void VTXApp::deleteAtEndOfFrame( const Component::Generic::BaseAutoDelete * const p_object )
		{
			_deleteAtEndOfFrameObjects.emplace_back( p_object );
		}
		void VTXApp::_applyEndOfFrameDeletes()
		{
			for ( const Component::Generic::BaseAutoDelete * const p_object : _deleteAtEndOfFrameObjects )
				p_object->autoDelete();

			_deleteAtEndOfFrameObjects.clear();
		}

	} // namespace App::Old

	App::Old::Application::RenderEffect::RenderEffectPreset & VTX_RENDER_EFFECT()
	{
		return App::Old::Application::RenderEffect::RenderEffectLibrary::get().getAppliedPreset();
	}

} // namespace VTX
